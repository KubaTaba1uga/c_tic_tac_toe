/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// App's internal libs
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_INPUT_REGISTRATIONS 10
#define INPUT_MODULE_ID "input_subsystem"

struct InputSubsystem {
  struct InputRegistrationData *registrations[MAX_INPUT_REGISTRATIONS];
  size_t count;
};

static struct InputSubsystem input_subsystem = {.count = 0};
static struct LoggingUtilsOps *logging_ops;
static struct StdLibUtilsOps *std_lib_ops;
static struct termios orginal_termios;

static int input_init(void);
static void input_wait(void);
static void input_destroy(void);
static void
input_register_module(struct InputRegistrationData *init_registration_data);
static int input_register_callback(char *id, input_callback_func_t callback);
static int input_unregister_callback(char *id);
static int input_start_non_blocking(void);
static void input_disable_canonical_mode(void);
static void input_restore_terminal_mode(void);
static void setup_signal_handlers(void);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InputOps input_ops = {
    .initialize = input_init,
    .destroy = input_destroy,
    .start = input_start_non_blocking,
    .wait = input_wait,
    .register_module = input_register_module,
    .register_callback = input_register_callback,
    .unregister_callback = input_unregister_callback,
};
struct InputOps *get_input_ops(void) { return &input_ops; };

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_input_reg = {
    .id = INPUT_MODULE_ID,
    .init = input_init,
    .destroy = input_destroy,
};

/*******************************************************************************
 *    API
 ******************************************************************************/
void input_register_module(
    struct InputRegistrationData *input_registration_data) {
  input_registration_data->callback = NULL;

  if (input_subsystem.count < MAX_INPUT_REGISTRATIONS) {
    input_subsystem.registrations[input_subsystem.count++] =
        input_registration_data;
  } else {
    logging_ops->log_err(INPUT_MODULE_ID,
                         "Unable to register %s in input, "
                         "no enough space in `registrations` array.",
                         input_registration_data->id);
  }
}

int input_register_callback(char *id, input_callback_func_t callback) {
  size_t i;

  for (i = 0; i < input_subsystem.count; ++i) {
    if (std_lib_ops->are_str_eq(id,
                                (char *)input_subsystem.registrations[i]->id)) {
      input_subsystem.registrations[i]->callback = callback;
      return 0;
    }
  }

  logging_ops->log_err(INPUT_MODULE_ID,
                       "Unable to register callback for %s, "
                       "no input module with this id",
                       id);

  return EINVAL;
}

int input_unregister_callback(char *id) {
  return input_register_callback(id, NULL);
}

int input_start_non_blocking(void) {
  size_t i;
  int err;

  for (i = 0; i < input_subsystem.count; ++i) {
    if (input_subsystem.registrations[i]->callback == NULL)
      continue;

    err = input_subsystem.registrations[i]->start();

    if (err != 0) {
      logging_ops->log_err(INPUT_MODULE_ID, "Unable to start module %s: %s",
                           input_subsystem.registrations[i]->id, strerror(err));
      return err;
    }
  }

  return 0;
}

void input_wait(void) {
  size_t i;
  for (i = 0; i < input_subsystem.count; ++i) {
    if (input_subsystem.registrations[i]->callback == NULL)
      continue;

    input_subsystem.registrations[i]->wait();
  }
}

int input_init(void) {
  logging_ops = get_logging_utils_ops();
  std_lib_ops = get_std_lib_utils_ops();

  // Disable canonical mode and echo, to receive input without pressing enter.
  // Register the terminal restoration function to run at exit
  atexit(input_restore_terminal_mode);

  // Setup signal handlers
  setup_signal_handlers();
  // Setup terminal
  input_disable_canonical_mode();

  return 0;
}

void input_destroy(void) {
  size_t i;
  input_restore_terminal_mode();

  for (i = 0; i < input_subsystem.count; ++i) {
    if (input_subsystem.registrations[i]->callback == NULL)
      continue;

    input_subsystem.registrations[i]->destroy();
    input_subsystem.registrations[i]->callback = NULL;
  }
}

// Function to restore the original terminal settings
void input_disable_canonical_mode(void) {
  struct termios new_termios;

  // Get the current terminal settings
  if (tcgetattr(STDIN_FILENO, &orginal_termios) == -1) {
    perror("tcgetattr");
    exit(EXIT_FAILURE);
  }

  // Disable canonical mode (ICANON) and echo (ECHO)
  new_termios = orginal_termios;
  new_termios.c_lflag &= ~(ICANON | ECHO);

  // Apply the new settings immediately
  if (tcsetattr(STDIN_FILENO, TCSANOW, &new_termios) == -1) {
    perror("tcsetattr");
    exit(EXIT_FAILURE);
  }
}

// Function to restore the original terminal settings
void input_restore_terminal_mode(void) {
  if (tcsetattr(STDIN_FILENO, TCSANOW, &orginal_termios) == -1) {
    perror("tcsetattr");
  }
}

// Signal handler to restore terminal settings and exit
void handle_signal(int sig) {
  // Restore terminal settings
  input_restore_terminal_mode();

  // Print the signal that caused termination
  fprintf(stderr, "Program terminated due to signal: %d\n", sig);

  // Re-raise the signal to terminate with the original signal handler
  signal(sig, SIG_DFL);
  raise(sig);
}

// Register the signal handlers for critical signals
void setup_signal_handlers() {
  struct sigaction sa;

  sa.sa_handler = handle_signal;
  /* sa.sa_flags = SA_RESTART; // Restart interrupted syscalls */
  sa.sa_flags = 0; // Restart interrupted syscalls
  sigemptyset(&sa.sa_mask);

  // Signals to handle
  int signals[] = {SIGINT, SIGTERM, SIGSEGV, SIGABRT};
  for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); ++i) {
    if (sigaction(signals[i], &sa, NULL) == -1) {
      perror("sigaction");
      exit(EXIT_FAILURE);
    }
  }
}
