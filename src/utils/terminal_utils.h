/*******************************************************************************
 * @file terminal_utils.h
 * @brief Terminal settings utility functions using the Ops pattern
 ******************************************************************************/

#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <unistd.h>

// Structure for terminal operations
struct TerminalUtilsOps {
  int (*enable_echo)(int fd);
  int (*disable_echo)(int fd);
  int (*restore_settings)(int fd);
  int (*enable_canonical_mode)(int fd);
  int (*disable_canonical_mode)(int fd);
  int (*get_terminal_dimensions)(int fd, int *rows, int *cols);
};

// Returns the terminal operations instance
struct TerminalUtilsOps *get_terminal_ops(void);

#endif // TERMINAL_UTILS_H
