/*******************************************************************************
 * @file keyboard.c
 * @brief Keyboard subsystem implementation
 *
 * This file contains the implementation of the keyboard subsystem which
 * initializes a thread to process stdin input and allows registering
 * callbacks to handle keyboard events.
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// App's internal libs
#include "keyboard.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define KEYBOARD_CALLBACK_MAX 10     // Maximum number of callbacks
#define KEYBOARD_STDIN_BUFFER_MAX 10 // Maximum size of stdin buffer
#define INPUT_KEYBOARD_ID "keyboard" // Identifier for logging

// Type definition for callback function pointers
typedef int (*callback_t)(void);

// Structure representing the keyboard subsystem
struct keyboard_subsystem {
  callback_t callbacks[KEYBOARD_CALLBACK_MAX];  // Array of callback functions
  size_t callback_count;                        // Count of registered callbacks
  char stdin_buffer[KEYBOARD_STDIN_BUFFER_MAX]; // Buffer to store stdin input
  size_t stdin_buffer_count; // Count of bytes in stdin buffer
  pthread_t thread;          // Thread for processing stdin input
};

static bool is_keyboard_initialized = false;
static struct keyboard_subsystem keyboard_subsystem;

static int keyboard_initialize(void);
static void *keyboard_process_stdin(void *);
static void keyboard_read_stdin(void);
static int keyboard_register_callback(callback_t callback);
static void keyboard_execute_callbacks(void);

struct keyboard_private_ops {
  void *(*process_stdin)(void *);
  void (*read_stdin)(void);
  void (*execute_callbacks)(void);
};

static struct keyboard_private_ops keyboard_private_ops = {
    .process_stdin = keyboard_process_stdin,
    .read_stdin = keyboard_read_stdin,
    .execute_callbacks = keyboard_execute_callbacks};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
// Structure for keyboard operations
struct keyboard_ops keyboard_ops = {.initialize = keyboard_initialize,
                                    .register_callback =
                                        keyboard_register_callback,
                                    .private = &keyboard_private_ops};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
/**
 * @brief Initialize the keyboard subsystem
 *
 * This function initializes the keyboard subsystem by creating a thread
 * to process stdin input. It also sets the initialized flag to true.
 *
 * @return 0 on success, error code on failure
 */
int keyboard_initialize(void) {
  if (is_keyboard_initialized) {
    logging_utils_ops.log_info(INPUT_KEYBOARD_ID,
                               "Keyboard is already initialized");
    return 0;
  }

  int err;

  // Clear the keyboard subsystem structure
  memset(&keyboard_subsystem, 0, sizeof(struct keyboard_subsystem));

  // Create the thread for processing stdin input
  err = pthread_create(&keyboard_subsystem.thread, NULL,
                       &keyboard_process_stdin, NULL);
  if (err) {
    logging_utils_ops.log_err(INPUT_KEYBOARD_ID,
                              "Unable to start keyboard_subsystem.thread: %s",
                              strerror(err));
    return err;
  }

  is_keyboard_initialized = true; // Set the initialized flag

  logging_utils_ops.log_info(INPUT_KEYBOARD_ID, "Initialized keyboard");

  return 0;
}

void *keyboard_process_stdin(void *_) {
  struct pollfd fds[1];
  int err;

  // Set the file descriptor and events to monitor
  fds[0].fd = STDIN_FILENO;
  fds[0].events = POLLIN;

  while (true) {
    // Wait for input on stdin
    err = poll(fds, 1, -1);

    if (err == -1) {
      logging_utils_ops.log_err(
          INPUT_KEYBOARD_ID, "Unable to wait for stdin: %s", strerror(errno));
      continue;
    }

    if (fds[0].revents & POLLIN) {
      keyboard_read_stdin();
    }
  }

  is_keyboard_initialized = false;

  pthread_exit(NULL); // Exit the thread
}

void keyboard_read_stdin(void) {
  ssize_t bytes_read;

  // Clear the stdin buffer count
  keyboard_subsystem.stdin_buffer_count = 0;

  // Read input from stdin into the buffer
  while (keyboard_subsystem.stdin_buffer_count <
         KEYBOARD_STDIN_BUFFER_MAX - 1) {

    bytes_read = read(
        STDIN_FILENO,
        keyboard_subsystem.stdin_buffer + keyboard_subsystem.stdin_buffer_count,
        KEYBOARD_STDIN_BUFFER_MAX - keyboard_subsystem.stdin_buffer_count - 1);

    printf("%zd bytes read\n", bytes_read);

    if (bytes_read < 0) {
      logging_utils_ops.log_err(INPUT_KEYBOARD_ID, "Unable to read from stdin");
      break; // Break on error
    } else if (bytes_read == 0) {
      // EOF
      break;
    }

    keyboard_subsystem.stdin_buffer_count += bytes_read;
  }

  // Ensure the buffer is null-terminated
  keyboard_subsystem.stdin_buffer[keyboard_subsystem.stdin_buffer_count] = '\0';
}

void keyboard_execute_callbacks(void) {
  size_t i;
  for (i = 0; i < keyboard_subsystem.callback_count; ++i) {
    if (keyboard_subsystem.callbacks[i]) {
      keyboard_subsystem.callbacks[i]();
    }
  }
}

/**
 * @brief Register a callback for keyboard events
 *
 * This function registers a callback that will be called when there is
 * input on stdin.
 *
 * @param callback The callback function to register
 * @return 0 on success, error code on failure
 */
int keyboard_register_callback(callback_t callback) {
  if (!callback) {
    return EINVAL;
  }

  if (keyboard_subsystem.callback_count < KEYBOARD_CALLBACK_MAX) {
    keyboard_subsystem.callbacks[keyboard_subsystem.callback_count++] =
        callback;
  } else {
    logging_utils_ops.log_err(INPUT_KEYBOARD_ID,
                              "Unable to register callback in keyboard, "
                              "no enough space in callbacks array.");
    return ENOMEM;
  }

  logging_utils_ops.log_info(INPUT_KEYBOARD_ID,
                             "Succesfully registered callback in keyboard.");

  return 0;
}
