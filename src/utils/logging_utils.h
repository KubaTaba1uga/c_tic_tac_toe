/**
 * @file logging_utils.h
 * @brief Logging utilities for managing loggers and logging messages.
 *
 * This header defines the interface for initializing, destroying, and logging
 * information or error messages using a set of logging utilities. The structure
 * `logging_utils_ops` contains function pointers for the operations related to
 * logging management.
 */
#ifndef _LOGGING_UTILS_H
#define _LOGGING_UTILS_H

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct logging_utils_ops {
  int (*init_loggers)(void);
  void (*destroy_loggers)(void);
  void (*log_info)(const char *msg_id, char *fmt, ...);
  void (*log_err)(const char *msg_id, char *fmt, ...);
  void *private;
};

extern struct logging_utils_ops logging_utils_ops;

#endif // _LOGGING_UTILS_H
