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

// App's internal libs
#include "init/init.h"
#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "keyboard.h"
#include "utils/logging_utils.h"
#include "utils/subsystem_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define KEYBOARD_CALLBACK_MAX 10
#define KEYBOARD_STDIN_BUFFER_MAX 10

struct KeyboardSubsystem {
  pthread_t thread;
  bool is_initialized;
  subsystem_t subsystem;
  size_t stdin_buffer_count;
  // Stdin buffer is not using array abstraction as array
  //   abstraction uses void * to provide genericness.
  //   We want to store pure chars in the array no ptrs to chars.
  char stdin_buffer[KEYBOARD_STDIN_BUFFER_MAX];
};

typedef struct KeyboardSubsystem *keyboard_sys_t;

struct KeyboardPrivateOps {
  int (*init)(keyboard_sys_t *);
  void (*destroy)(keyboard_sys_t *);
  void (*read_stdin)(keyboard_sys_t);
  int (*start_thread)(keyboard_sys_t);
  void (*stop_thread)(keyboard_sys_t);
  void *(*process_stdin)(keyboard_sys_t);
  void (*execute_callbacks)(keyboard_sys_t);
  int (*register_callback)(keyboard_sys_t, struct KeyboardRegistration *);
};

static keyboard_sys_t keyboard_subsystem;
static const char module_id[] = "keyboard";
static struct LoggingUtilsOps *logging_ops;
static struct SubsystemUtilsOps *subsystem_ops;

static struct KeyboardPrivateOps *keyboard_priv_ops;
struct KeyboardPrivateOps *get_keyboard_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int keyboard_init_system(void) {
  int err;

  logging_ops = get_logging_utils_ops();
  subsystem_ops = get_subsystem_utils_ops();
  keyboard_priv_ops = get_keyboard_priv_ops();

  if (keyboard_subsystem) {
    logging_ops->log_info(module_id, "Keyboard subsystem already initialized.");
    return 0;
  }

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

  err = keyboard_priv_ops->start_thread(keyboard_subsystem);
  if (err) {
    logging_ops->log_err(module_id, "Failed to start keyboard thread: %s",
                         strerror(err));
    return err;
  }

  logging_ops->log_info(module_id, "Keyboard thread started.");
  return 0;
}

static void keyboard_stop_thread_system(void) {
  keyboard_priv_ops->stop_thread(keyboard_subsystem);

  logging_ops->log_info(module_id, "Keyboard thread stopped.");
}

static int
keyboard_register_callback_system(struct KeyboardRegistration *keyboard_reg) {
  int err;

  err = keyboard_priv_ops->register_callback(keyboard_subsystem, keyboard_reg);
  if (err) {
    logging_ops->log_err(module_id, "Failed to register keyboard callback: %s",
                         strerror(err));
    return err;
  }

  logging_ops->log_info(module_id, "Keyboard callback registered.");
  return 0;
}

static int keyboard_wait_system(void) {
  if (!keyboard_subsystem->is_initialized) {
    logging_ops->log_info(
        module_id, "Keyboard subsystem is not running. Nothing to wait for.");
    return 0; // Nothing to wait for; return success
  }

  logging_ops->log_info(module_id, "Waiting for keyboard thread to finish.");

  if (keyboard_subsystem->thread) {
    int err = pthread_join(keyboard_subsystem->thread, NULL);
    if (err) {
      logging_ops->log_err(module_id, "Failed to join keyboard thread: %s",
                           strerror(err));
      return err;
    }
    keyboard_subsystem->thread = 0;
  }

  logging_ops->log_info(module_id,
                        "Keyboard thread has finished successfully.");
  return 0; // Success
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int keyboard_init(keyboard_sys_t *keyboard) {
  keyboard_sys_t tmp_keyboard;
  int err;

  if (!keyboard)
    return EINVAL;

  tmp_keyboard = malloc(sizeof(struct KeyboardSubsystem));
  if (!tmp_keyboard) {
    logging_ops->log_err(module_id,
                         "Unable to allocate memory for keyboard subsystem: %s",
                         strerror(ENOMEM));
    return ENOMEM;
  }

  memset(tmp_keyboard, 0, sizeof(struct KeyboardSubsystem));
  err = subsystem_ops->init(&tmp_keyboard->subsystem, module_id,
                            KEYBOARD_CALLBACK_MAX);
  if (err) {
    free(tmp_keyboard);
    logging_ops->log_err(module_id,
                         "Unable to create subsystem for keyboard: %s",
                         strerror(err));
    return err;
  }

  tmp_keyboard->is_initialized = false;

  *keyboard = tmp_keyboard;

  return 0;
}

static void keyboard_destroy(keyboard_sys_t *keyboard) {
  keyboard_sys_t tmp_keyboard;

  if (!keyboard || !*keyboard)
    return;

  tmp_keyboard = *keyboard;

  subsystem_ops->destroy(&tmp_keyboard->subsystem);

  free(tmp_keyboard);
  *keyboard = NULL;
}

static int keyboard_start_thread(keyboard_sys_t keyboard) {
  int err;

  if (!keyboard || keyboard->is_initialized)
    return EINVAL;

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

static void keyboard_stop_thread(keyboard_sys_t keyboard) {
  if (!keyboard || !keyboard->is_initialized)
    return;

  keyboard->is_initialized = false;

  if (keyboard->thread) {
    pthread_kill(keyboard->thread, SIGUSR1);
    pthread_join(keyboard->thread, NULL);
    keyboard->thread = 0;
  }
}

static void keyboard_read_stdin(keyboard_sys_t keyboard) {
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
  keyboard->stdin_buffer[bytes_read] = '\0';
}

static bool keyboard_array_search_filter(const char *_, void *__, void *___) {
  return true;
}

static void keyboard_execute_callbacks(keyboard_sys_t keyboard) {
  struct KeyboardRegistration *keyboard_reg;
  module_search_t search_wrap;
  int err;

  if (!keyboard || !keyboard->is_initialized)
    return;

  err = subsystem_ops->init_search_module_wrapper(
      &search_wrap, NULL, keyboard_array_search_filter, (void **)&keyboard_reg);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init search wrapper: %s",
                         strerror(err));
    goto out;
  }

  do {
    err = subsystem_ops->search_modules(keyboard->subsystem, search_wrap);
    if (err) {
      logging_ops->log_err(module_id, "Unable find callback for keyboard: %s",
                           strerror(err));
      goto cleanup;
    }
    if (!keyboard_reg)
      break;

    keyboard_reg->callback(keyboard->stdin_buffer_count,
                           keyboard->stdin_buffer);

    logging_ops->log_info(module_id, "Executed callback for %s",
                          keyboard_reg->module_id);
  } while (true);

  logging_ops->log_info(module_id, "Executed all callbacks");

cleanup:
  subsystem_ops->destroy_search_module_wrapper(&search_wrap);
out:
  return;
}

static int
keyboard_register_callback(keyboard_sys_t keyboard,
                           struct KeyboardRegistration *keyboard_reg) {
  int err;

  if (!keyboard || !keyboard_reg || !keyboard_reg->callback ||
      !keyboard_reg->module_id) {
    logging_ops->log_err(module_id, "Invalid keyboard registration data.");
    return EINVAL;
  }

  err = subsystem_ops->register_module(keyboard->subsystem,
                                       keyboard_reg->module_id,
                                       (void *)keyboard_reg->callback);
  if (err) {
    return err;
  }

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

static void *keyboard_process_stdin(keyboard_sys_t keyboard) {
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
struct InitRegistrationData init_keyboard_reg = {
    .id = module_id,
    .init = keyboard_init_system,
    .destroy = keyboard_destroy_system,
};

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
