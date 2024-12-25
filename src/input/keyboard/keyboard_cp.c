/*******************************************************************************
 * @file keyboard.c
 * @brief Refactored Keyboard subsystem implementation
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "static_array_lib.h"

// App's internal libs
#include "init/init.h"
#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "utils/logging_utils.h"
#include "utils/terminal_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define KEYBOARD_KEYS_MAPPINGS_MAX 10
#define KEYBOARD_STDIN_BUFFER_MAX 10

typedef struct KeyboardSubsystem {
  pthread_t thread;
  bool is_initialized;
  size_t stdin_buffer_count;
  char stdin_buffer[KEYBOARD_STDIN_BUFFER_MAX];
  SARRS_FIELD(keys_mappings, struct KeyboardKeysMapping,
              KEYBOARD_KEYS_MAPPINGS_MAX);
} KeyboardSubsystem;

SARRS_DECL(KeyboardSubsystem, keys_mappings, struct KeyboardKeysMapping,
           KEYBOARD_KEYS_MAPPINGS_MAX);

struct KeyboardPrivateOps {
  int (*init)(struct KeyboardSubsystem *);
  void (*destroy)(struct KeyboardSubsystem *);
  void (*read_stdin)(struct KeyboardSubsystem *);
  int (*start_thread)(struct KeyboardSubsystem *);
  void (*stop_thread)(struct KeyboardSubsystem *);
  void *(*process_stdin)(struct KeyboardSubsystem *);
  void (*execute_callbacks)(struct KeyboardSubsystem *);
  int (*register_callback)(struct KeyboardAddKeysMappingInput *,
                           struct KeyboardAddKeysMappingOutput *);
};

static struct RegistrationUtilsOps *reg_ops;
static const char module_id[] = "keyboard";
static struct LoggingUtilsOps *logging_ops;
static struct TerminalUtilsOps *terminal_ops;
static struct KeyboardSubsystem keyboard_subsystem;
static struct KeyboardPrivateOps *keyboard_priv_ops;
struct KeyboardPrivateOps *get_keyboard_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int keyboard_init_system(void) {
  int err;

  terminal_ops = get_terminal_ops();
  logging_ops = get_logging_utils_ops();
  keyboard_priv_ops = get_keyboard_priv_ops();

  err = keyboard_priv_ops->init(&keyboard_subsystem);
  if (err) {
    logging_ops->log_err(module_id,
                         "Failed to initialize keyboard subsystem: %s",
                         strerror(err));
    return err;
  }

  return 0;
}

static void keyboard_destroy_system(void) {

  keyboard_priv_ops->destroy(&keyboard_subsystem);
}

static int keyboard_start_thread_system(void) {
  int err;

  err = keyboard_priv_ops->start_thread(&keyboard_subsystem);
  if (err) {
    logging_ops->log_err(module_id, "Failed to start keyboard thread: %s",
                         strerror(err));
    return err;
  }

  logging_ops->log_info(module_id, "Keyboard thread started.");
  return 0;
}

static void keyboard_stop_thread_system(void) {
  keyboard_priv_ops->stop_thread(&keyboard_subsystem);

  logging_ops->log_info(module_id, "Keyboard thread stopped.");
}

static int
keyboard_register_callback_system(struct KeyboardAddKeysMappingInput input,
                                  struct KeyboardAddKeysMappingOutput *output) {
  int err;

  if (!output)
    return EINVAL;

  input.keyboard = &keyboard_subsystem;

  err = keyboard_priv_ops->register_callback(&input, output);
  if (err) {
    logging_ops->log_err(module_id, "Failed to register callback for %s: %s.",
                         input.registration->registration.display_name,
                         strerror(err));
    return err;
  }

  logging_ops->log_info(module_id, "Registered callback for %s.",
                        input.registration->registration.display_name);
  return 0;
}

static int keyboard_wait_system(void) {
  if (!keyboard_subsystem.is_initialized) {
    logging_ops->log_info(
        module_id, "Keyboard subsystem is not running. Nothing to wait for.");
    return 0;
  }

  logging_ops->log_info(module_id, "Waiting for keyboard thread to finish.");

  if (keyboard_subsystem.thread) {
    int err = pthread_join(keyboard_subsystem.thread, NULL);
    if (err) {
      logging_ops->log_err(module_id, "Failed to join keyboard thread: %s",
                           strerror(err));
      return err;
    }
    keyboard_subsystem.thread = 0;
  }

  logging_ops->log_info(module_id,
                        "Keyboard thread has finished successfully.");
  return 0;
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int keyboard_init(struct KeyboardSubsystem *keyboard) {
  int err;

  if (!keyboard)
    return EINVAL;

  err =
      reg_ops->init(&keyboard->registrar, __FILE_NAME__, KEYBOARD_CALLBACK_MAX);
  if (err) {
    logging_ops->log_err(module_id, "Unable to initialize registrar: %s",
                         strerror(err));
    return err;
  }

  keyboard->is_initialized = false;

  terminal_ops->disable_canonical_mode(STDIN_FILENO);

  return 0;
}

static void keyboard_destroy(struct KeyboardSubsystem *keyboard) {
  if (!keyboard || !keyboard->is_initialized)
    return;

  reg_ops->destroy(&keyboard->registrar);
  keyboard->is_initialized = false;

  terminal_ops->enable_canonical_mode(STDIN_FILENO);
}

static int keyboard_start_thread(struct KeyboardSubsystem *keyboard) {
  int err;

  if (!keyboard)
    return EINVAL;

  if (keyboard->is_initialized)
    return 0;

  keyboard->is_initialized = true;
  err = pthread_create(&keyboard->thread, NULL,
                       (void *)keyboard_priv_ops->process_stdin, keyboard);
  if (err) {
    keyboard->is_initialized = false;
    logging_ops->log_err(module_id, "Unable to start keyboard thread: %s",
                         strerror(err));
    return err;
  }

  return 0;
}

static void keyboard_stop_thread(struct KeyboardSubsystem *keyboard) {
  if (!keyboard || !keyboard->is_initialized)
    return;

  keyboard->is_initialized = false;

  if (keyboard->thread) {
    pthread_kill(keyboard->thread, SIGUSR1);
    pthread_join(keyboard->thread, NULL);
    keyboard->thread = 0;
  }
}

static void keyboard_read_stdin(struct KeyboardSubsystem *keyboard) {
  ssize_t bytes_read;

  if (!keyboard || !keyboard->is_initialized)
    return;

  bytes_read =
      read(STDIN_FILENO, keyboard->stdin_buffer, KEYBOARD_STDIN_BUFFER_MAX - 1);

  if (bytes_read < 0) {
    logging_ops->log_err(module_id, "Unable to read from stdin: %s",
                         strerror(errno));
    return;
  }

  keyboard->stdin_buffer_count = bytes_read;

  if (bytes_read != 0) {
    keyboard->stdin_buffer[bytes_read] = '\0';
  }
}

static void keyboard_execute_callbacks(struct KeyboardSubsystem *keyboard) {
  struct GetRegistrationInput input;
  struct GetRegistrationOutput output;
  struct KeyboardRegistrationData *reg_data;
  size_t i;
  int err;

  if (!keyboard || !keyboard->is_initialized)
    return;

  input.registrar = &keyboard->registrar;

  for (i = 0; i < input.registrar->registrations.length; i++) {
    input.registration_id = i;

    err = reg_ops->get_registration(input, &output);
    if (err) {
      logging_ops->log_err(module_id, "Unable find callback for keyboard: %s",
                           strerror(err));
      return;
    }

    reg_data = output.registration->private;
    err = reg_data->callback(keyboard->stdin_buffer_count,
                             keyboard->stdin_buffer);
    if (err) {
      logging_ops->log_err(module_id,
                           "Unable to execute callback for keyboard: %s",
                           strerror(err));
      return;
    }

    logging_ops->log_info(module_id, "Executed callback for %s",
                          output.registration->display_name);
  };

  logging_ops->log_info(module_id, "Executed all callbacks");

  return;
}

static int keyboard_register_callback(
    struct KeyboardAddKeysMappingInput *keyboard_input,
    struct KeyboardAddKeysMappingOutput *keyboard_output) {
  struct KeyboardSubsystem *keyboard;
  struct RegisterOutput reg_output;

  int err;

  keyboard = keyboard_input->keyboard;

  err = reg_ops->register_module(
      (struct RegisterInput){.registration =
                                 &keyboard_input->registration->registration,
                             .registrar = &keyboard->registrar},
      &reg_output);
  if (err) {
    return err;
  }

  keyboard_output->registration_id = reg_output.registration_id;

  return 0;
}

void keyboard_signal_handler(int sig) {
  // You can use SIGUSR1 to interrupt or terminate all input threads.
  if (sig == SIGUSR1) {
    logging_ops->log_info(module_id,
                          "Thread received signal SIGUSR1. Exiting.");

    pthread_exit(NULL);
  }
}

static void *keyboard_process_stdin(struct KeyboardSubsystem *keyboard) {
  if (!keyboard || !keyboard->is_initialized) {
    logging_ops->log_err(
        module_id, "Keyboard subsystem is not initialized. Exiting thread.");
    return NULL;
  }

  // Handle signal to terminate the thread
  signal(SIGUSR1, keyboard_signal_handler);

  logging_ops->log_info(module_id, "Keyboard processing thread started.");

  while (keyboard->is_initialized) {
    // Reuse `keyboard_read_stdin` to process input
    keyboard_priv_ops->read_stdin(keyboard);

    // Execute registered callbacks after processing input
    keyboard_priv_ops->execute_callbacks(keyboard);
  }

  logging_ops->log_info(module_id, "Keyboard processing thread exiting.");
  return NULL;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistration init_keyboard_reg = {
    .data = {.display_name = __FILE_NAME__,
             .init = keyboard_init_system,
             .destroy = keyboard_destroy_system}};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct KeyboardOps keyboard_ops = {
    .wait = keyboard_wait_system,
    .stop = keyboard_stop_thread_system,
    .start = keyboard_start_thread_system,
    .register_callback = keyboard_register_callback_system,
};

static struct KeyboardPrivateOps keyboard_priv_ops_ = {
    .init = keyboard_init,
    .destroy = keyboard_destroy,
    .read_stdin = keyboard_read_stdin,
    .stop_thread = keyboard_stop_thread,
    .start_thread = keyboard_start_thread,
    .process_stdin = keyboard_process_stdin,
    .execute_callbacks = keyboard_execute_callbacks,
    .register_callback = keyboard_register_callback,
};

struct KeyboardPrivateOps *get_keyboard_priv_ops(void) {
  return &keyboard_priv_ops_;
}

struct KeyboardOps *get_keyboard_ops(void) { return &keyboard_ops; }
