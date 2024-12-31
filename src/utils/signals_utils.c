/*******************************************************************************
 * @file signal_utils.c
 * @brief Signal handling utilities to manage multiple callbacks for signals.
 ******************************************************************************/

#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "static_array_lib.h"

#include "utils/signals_utils.h"

#define MAX_SIGNAL_HANDLERS 10

typedef void (*signal_callback_t)(void);

struct SignalsSubsystem {
  SARRS_FIELD(callbacks, signal_callback_t, MAX_SIGNAL_HANDLERS);
};

typedef struct SignalsSubsystem SignalsSubsystem;

SARRS_DECL(SignalsSubsystem, callbacks, signal_callback_t, MAX_SIGNAL_HANDLERS);

SignalsSubsystem signals_subsystem;

static void signals_utils_execute_callbacks(void) {
  signal_callback_t *callback;
  for (size_t i = 0; i < SignalsSubsystem_callbacks_length(&signals_subsystem);
       ++i) {
    SignalsSubsystem_callbacks_get(&signals_subsystem, i, &callback);

    (*callback)();
  }
}

static void signals_utils_signal_handler(int sig) {
  signals_utils_execute_callbacks();
  signal(sig, SIG_DFL); // Reset the signal to its default behavior
  raise(sig);           // Re-raise the signal to terminate the program
}

int signal_utils_add_handler(signal_callback_t callback) {
  int err;

  err = SignalsSubsystem_callbacks_append(&signals_subsystem, callback);
  if (err) {
    return err;
  }

  return 0;
}

static int signal_utils_register_signals(void) {
  struct sigaction sa;

  sa.sa_handler = signals_utils_signal_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  atexit(signals_utils_execute_callbacks);

  // Hardcoded signals to register
  int signals[] = {SIGINT, SIGTERM, SIGSEGV, SIGABRT};

  for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); ++i) {
    if (sigaction(signals[i], &sa, NULL) == -1) {
      perror("sigaction");
      return -1;
    }
  }

  return 0;
}

static struct SignalUtilsOps signal_utils_ops = {
    .init = signal_utils_register_signals,
    .add_handler = signal_utils_add_handler,
};

struct SignalUtilsOps *get_signal_utils_ops(void) { return &signal_utils_ops; }