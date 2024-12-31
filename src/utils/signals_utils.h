/*******************************************************************************
 * @file signal_utils.c
 * @brief Signal handling utilities to manage multiple callbacks for signals.
 *
 * This file provides functionality for registering and managing callbacks
 * for signals. It includes mechanisms to handle multiple signal handlers
 * and a subsystem to ensure proper execution of registered callbacks when
 * a signal is received.
 ******************************************************************************/

#ifndef SIGNAL_UTILS_H
#define SIGNAL_UTILS_H

#include <signal.h>
#include <stddef.h>

typedef void (*signal_callback_t)(void);

struct SignalUtilsOps {
  int (*init)(void);
  int (*add_handler)(signal_callback_t callback);
};

struct SignalUtilsOps *get_signal_utils_ops(void);

#endif // SIGNAL_UTILS_H
