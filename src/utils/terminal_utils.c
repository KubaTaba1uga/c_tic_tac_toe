/*******************************************************************************
 * @file terminal_utils.c
 * @brief Terminal settings utility functions using the Ops pattern
 ******************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "utils/terminal_utils.h"

// Static storage for original terminal settings
static struct termios original_termios;

// Function to disable canonical mode
static int disable_canonical_mode(int fd) {
  struct termios new_termios;

  // Save the current terminal settings
  if (tcgetattr(fd, &original_termios) == -1) {
    perror("tcgetattr");
    return errno;
  }

  // Disable canonical mode
  new_termios = original_termios;
  new_termios.c_lflag &= ~ICANON;

  // Apply the new settings
  if (tcsetattr(fd, TCSANOW, &new_termios) == -1) {
    perror("tcsetattr");
    return errno;
  }

  return 0; // Success
}

// Function to enable canonical mode
static int enable_canonical_mode(int fd) {
  struct termios new_termios;

  // Get the current terminal settings
  if (tcgetattr(fd, &new_termios) == -1) {
    perror("tcgetattr");
    return errno;
  }

  // Enable canonical mode
  new_termios.c_lflag |= ICANON;

  // Apply the new settings
  if (tcsetattr(fd, TCSANOW, &new_termios) == -1) {
    perror("tcsetattr");
    return errno;
  }

  return 0; // Success
}

// Function to disable echo
static int disable_echo(int fd) {
  struct termios new_termios;

  // Save the current terminal settings
  if (tcgetattr(fd, &original_termios) == -1) {
    perror("tcgetattr");
    return errno;
  }

  // Disable echo
  new_termios = original_termios;
  new_termios.c_lflag &= ~ECHO;

  // Apply the new settings
  if (tcsetattr(fd, TCSANOW, &new_termios) == -1) {
    perror("tcsetattr");
    return errno;
  }

  return 0; // Success
}

// Function to enable echo
static int enable_echo(int fd) {
  struct termios new_termios;

  // Get the current terminal settings
  if (tcgetattr(fd, &new_termios) == -1) {
    perror("tcgetattr");
    return errno;
  }

  // Enable echo
  new_termios.c_lflag |= ECHO;

  // Apply the new settings
  if (tcsetattr(fd, TCSANOW, &new_termios) == -1) {
    perror("tcsetattr");
    return errno;
  }

  return 0; // Success
}

// Function to restore original terminal settings
static int restore_settings(int fd) {
  if (tcsetattr(fd, TCSANOW, &original_termios) == -1) {
    perror("tcsetattr");
    return errno;
  }

  return 0; // Success
}

// Function to get terminal dimensions
static int get_terminal_dimensions(int fd, int *rows, int *cols) {
  struct winsize ws;

  if (ioctl(fd, TIOCGWINSZ, &ws) == -1) {
    perror("ioctl");
    return errno;
  }

  if (rows) {
    *rows = ws.ws_row;
  }
  if (cols) {
    *cols = ws.ws_col;
  }

  return 0; // Success
}

// Terminal operations instance
static struct TerminalUtilsOps terminal_ops = {
    .disable_canonical_mode = disable_canonical_mode,
    .enable_canonical_mode = enable_canonical_mode,
    .disable_echo = disable_echo,
    .enable_echo = enable_echo,
    .restore_settings = restore_settings,
    .get_terminal_dimensions = get_terminal_dimensions};

// Returns the terminal operations instance
struct TerminalUtilsOps *get_terminal_ops(void) {
  return &terminal_ops;
}
