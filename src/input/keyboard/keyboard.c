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
#include "input/input_common.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_keys_mapping.h"
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
  int (*add_keys_mapping)(struct KeyboardAddKeysMappingInput *,
                          struct KeyboardAddKeysMappingOutput *);
};

static struct InputOps *input_ops;
static const char module_id[] = "keyboard";
static struct LoggingUtilsOps *logging_ops;
static struct TerminalUtilsOps *terminal_ops;
static struct KeyboardSubsystem keyboard_subsystem;
static struct KeyboardPrivateOps *keyboard_priv_ops;
struct KeyboardPrivateOps *get_keyboard_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int keyboard_init_intrfc(void) {
  int err;

  terminal_ops = get_terminal_ops();
  logging_ops = get_logging_utils_ops();
  keyboard_priv_ops = get_keyboard_priv_ops();
  input_ops = get_input_ops();

  err = keyboard_priv_ops->init(&keyboard_subsystem);
  if (err) {
    logging_ops->log_err(module_id,
                         "Failed to initialize keyboard subsystem: %s",
                         strerror(err));
    return err;
  }

  return 0;
}

static int keyboard_start_thread_intrfc(void) {
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

static int keyboard_stop_thread_intrfc(void) {
  keyboard_priv_ops->stop_thread(&keyboard_subsystem);

  logging_ops->log_info(module_id, "Keyboard thread stopped.");

  return 0;
}

static int
keyboard_add_keys_mapping_intrfc(struct KeyboardAddKeysMappingInput *input,
                                 struct KeyboardAddKeysMappingOutput *output) {
  int err;

  if (!output)
    return EINVAL;

  input->private = &keyboard_subsystem;

  err = keyboard_priv_ops->add_keys_mapping(input, output);
  if (err) {
    logging_ops->log_err(module_id,
                         "Failed to add keys mapping callback for %s: %s",
                         input->keys_mapping->display_name, strerror(err));
    return err;
  }

  logging_ops->log_info(module_id, "Added keys mapping %s",
                        input->keys_mapping->display_name);
  return 0;
}

static int keyboard_wait_intrfc(void) {
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

static void keyboard_destroy_intrfc(void) {
  // Check if the keyboard subsystem is initialized
  if (keyboard_subsystem.is_initialized) {
    // Stop the keyboard thread if running
    keyboard_stop_thread_intrfc();
  }

  // Perform any additional cleanup using private destroy function
  keyboard_priv_ops->destroy(&keyboard_subsystem);

  // Log the successful destruction
  logging_ops->log_info(module_id,
                        "Keyboard subsystem destroyed successfully.");
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int keyboard_init(struct KeyboardSubsystem *keyboard) {
  if (!keyboard)
    return EINVAL;

  KeyboardSubsystem_keys_mappings_init(keyboard);

  keyboard->is_initialized = false;

  terminal_ops->disable_canonical_mode(STDIN_FILENO);

  return 0;
}

static void keyboard_destroy(struct KeyboardSubsystem *keyboard) {
  if (!keyboard || !keyboard->is_initialized)
    return;

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
  struct KeyboardKeysMappingCallbackOutput keyboard_callback_output;
  struct InputGetDeviceOutput get_device_output;
  struct KeyboardKeysMapping *keys_mapping;
  size_t i;
  int err;

  if (!keyboard || !keyboard->is_initialized)
    return;

  for (i = 0; i < KeyboardSubsystem_keys_mappings_length(keyboard); i++) {
    logging_ops->log_info(module_id, "I: %i", i);

    err = KeyboardSubsystem_keys_mappings_get(keyboard, i, &keys_mapping);
    if (err) {
      logging_ops->log_err(module_id, "Unable to find keys mapping for %d: %s",
                           i, strerror(err));
      return;
    }

    memset(&get_device_output, 0, sizeof(struct InputGetDeviceOutput));

    err = input_ops->get_device(
        (struct InputGetDeviceInput){.device_id = keys_mapping->device_id},
        &get_device_output);
    if (err) {
      logging_ops->log_err(module_id, "Getting input device failed for %s: %s",
                           keys_mapping->display_name, strerror(err));
      return;
    }

    if (!get_device_output.device->callback) {
      logging_ops->log_info(module_id, "No input callback in keys mappings %s",
                            keys_mapping->display_name);
      continue;
    }

    memset(&keyboard_callback_output, 0,
           sizeof(struct KeyboardKeysMappingCallbackOutput));

    err = keys_mapping->callback(
        &(struct KeyboardKeysMappingCallbackInput){
            .buffer = keyboard->stdin_buffer,
            .n = keyboard->stdin_buffer_count},
        &keyboard_callback_output);
    if (err) {
      logging_ops->log_err(
          module_id,
          "Unable to process keyboard callback for keys mappings %s: %s",
          keys_mapping->display_name, strerror(err));

      return;
    }

    logging_ops->log_info(module_id, "Executed keyboard callback for %s",
                          keys_mapping->display_name);

    if (keyboard_callback_output.input_event == INPUT_EVENT_NONE) {
      logging_ops->log_info(module_id,
                            "No input event in keys mappings %s callback",
                            keys_mapping->display_name);
      continue;
    }

    err = get_device_output.device->callback(
        keyboard_callback_output.input_event, keys_mapping->device_id);
    if (err) {
      logging_ops->log_err(
          module_id, "Unable to process input callback for keys mapping %s: %s",
          keys_mapping->display_name, strerror(err));
      return;
    }

    logging_ops->log_info(module_id, "Executed input callback for %s",
                          keys_mapping->display_name);
  }

  logging_ops->log_info(module_id, "Executed all callbacks");

  return;
}

static int
keyboard_add_keys_mapping(struct KeyboardAddKeysMappingInput *input,
                          struct KeyboardAddKeysMappingOutput *output) {
  struct KeyboardSubsystem *keyboard;
  int err;

  if (!input || !input->keys_mapping || !output) {
    return EINVAL;
  }

  keyboard = input->private;

  err = KeyboardSubsystem_keys_mappings_append(keyboard, *input->keys_mapping);
  if (err) {
    return err;
  }

  output->keys_mapping_id =
      KeyboardSubsystem_keys_mappings_length(keyboard) - 1;

  return 0;
}

static void keyboard_signal_handler(int sig) {
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
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct KeyboardOps keyboard_ops = {
    .init = keyboard_init_intrfc,
    .destroy = keyboard_destroy_intrfc,
    .wait = keyboard_wait_intrfc,
    .stop = keyboard_stop_thread_intrfc,
    .start = keyboard_start_thread_intrfc,
    .add_keys_mapping = keyboard_add_keys_mapping_intrfc,
};

static struct KeyboardPrivateOps keyboard_priv_ops_ = {
    .init = keyboard_init,
    .destroy = keyboard_destroy,
    .read_stdin = keyboard_read_stdin,
    .stop_thread = keyboard_stop_thread,
    .start_thread = keyboard_start_thread,
    .process_stdin = keyboard_process_stdin,
    .execute_callbacks = keyboard_execute_callbacks,
    .add_keys_mapping = keyboard_add_keys_mapping,
};

struct KeyboardPrivateOps *get_keyboard_priv_ops(void) {
  return &keyboard_priv_ops_;
}

struct KeyboardOps *get_keyboard_ops(void) { return &keyboard_ops; }
