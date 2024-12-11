/*******************************************************************************
 * @file cli.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/
// TO-DO pass to display matrix 3x3 and some metadata.
//        matrix is easier to translate to image than list
//        because you can iterate over rows and columns.

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// App's internal libs
#include "display/cli.h"
#include "display/display.h"
#include "game/game.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "init/init.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct CliDisplay {
  size_t width;
  size_t height;
};

static struct DisplayRegistrationData cli_display_reg_data;

static char module_id[] = "cli";
struct termios orginal_termios;

static int cli_display(struct DataToDisplay *data);
static int cli_get_terminal_size(struct CliDisplay *data);
static int cli_format_game_row(struct CliDisplay *display,
                               struct DataToDisplay *data, size_t row_no,
                               size_t buffer_size, char buffer[buffer_size]);
static void cli_sort_user_moves(int n, struct UserMove user_moves[n]);
static int cli_module_init(void);
static void cli_module_destroy(void);
static int cli_init_display(struct CliDisplay *data);
static int cli_configure_terminal(void);
static void cli_terminal_disable_echo_and_cannonical(void);
static void cli_restore_terminal(void);
static void cli_setup_signal_handlers();

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct CliDisplayPrivateOps {
  display_display_func_t display;
  int (*init_display)(struct CliDisplay *data);
  int (*get_terminal_size)(struct CliDisplay *data);
  void (*sort_user_moves)(int n, struct UserMove user_moves[n]);
  int (*format_game_row)(struct CliDisplay *display, struct DataToDisplay *data,
                         size_t row_no, size_t buffer_size,
                         char buffer[buffer_size]);
};

struct CliDisplayPrivateOps priv_ops = {
    .display = cli_display,
    .get_terminal_size = cli_get_terminal_size,
    .format_game_row = cli_format_game_row,
    .sort_user_moves = cli_sort_user_moves,
    .init_display = cli_init_display,
};

static struct DisplayCliOps cli_display_ops = {.private_ops = &priv_ops};

struct DisplayCliOps *get_display_cli_ops(void) { return &cli_display_ops; }

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_cli_reg = {
    .id = module_id,
    .init = cli_module_init,
    .destroy = cli_module_destroy,
};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int cli_module_init(void) {
  struct DisplayOps *display_ops;

  display_ops = get_display_ops();

  cli_display_reg_data.display = priv_ops.display;
  cli_display_reg_data.id = module_id;

  cli_configure_terminal();

  display_ops->register_module(&cli_display_reg_data);

  return 0;
};
static void cli_module_destroy(void){};

int cli_configure_terminal(void) {
  // Disable canonical mode and echo, to receive input without pressing enter.
  // Register the terminal restoration function to run at exit
  atexit(cli_restore_terminal);

  // Setup signal handlers
  cli_setup_signal_handlers();

  // Setup terminal
  cli_terminal_disable_echo_and_cannonical();

  return 0;
}

// Function to restore the original terminal settings
void cli_terminal_disable_echo_and_cannonical(void) {
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
void cli_restore_terminal(void) {
  if (tcsetattr(STDIN_FILENO, TCSANOW, &orginal_termios) == -1) {
    perror("tcsetattr");
  }
}

// Signal handler to restore terminal settings and exit
void cli_handle_signal(int sig) {
  // Restore terminal settings
  cli_restore_terminal();

  // Print the signal that caused termination
  fprintf(stderr, "Program terminated due to signal: %d\n", sig);

  // Re-raise the signal to terminate with the original signal handler
  signal(sig, SIG_DFL);
  raise(sig);
}

// Register the signal handlers for critical signals
void cli_setup_signal_handlers() {
  struct sigaction sa;

  sa.sa_handler = cli_handle_signal;
  /* sa.sa_flags = SA_RESTART; // Restart interrupted syscalls */
  sa.sa_flags = 0; // Do not restart interrupted syscalls
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

int cli_display(struct DataToDisplay *data) {
  struct LoggingUtilsOps *logging_ops;
  struct CliDisplay display;
  size_t buffer_size = 1024;
  char buffer[buffer_size];
  int err;

  logging_ops = get_logging_utils_ops();

  err = priv_ops.init_display(&display);
  if (err) {
    logging_ops->log_err(module_id, "Unable to initialize cli display.");
    return err;
  }

  for (size_t i = 0; i < 3; i++) {
    memset(buffer, 0, buffer_size - 1);
    err = cli_format_game_row(&display, data, i, buffer_size, buffer);
    if (err) {
      logging_ops->log_err(module_id, "Unable to format (%i) cli row.", i);
      return err;
    }
    printf("%s\n", buffer);
  }

  return 0;
};

int cli_init_display(struct CliDisplay *data) {
  struct LoggingUtilsOps *logging_ops;
  int err;

  logging_ops = get_logging_utils_ops();

  err = priv_ops.get_terminal_size(data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to get terminal size.");
    return err;
  }

  return 0;
}

int cli_get_terminal_size(struct CliDisplay *data) {
  struct winsize w;

  // Use ioctl to get terminal size
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
    perror("ioctl");
    return 1; // Return an error code if ioctl fails
  }

  data->width = w.ws_col;
  data->height = w.ws_row;

  return 0;
}

int cli_format_game_row(struct CliDisplay *display, struct DataToDisplay *data,
                        size_t row_no, size_t buffer_size,
                        char buffer[buffer_size]) {

  struct UserMove row_user_moves[100];
  size_t row_moves_counter = 0;
  size_t buffer_counter = 0;
  size_t i;

  // Find moves matching the row
  for (i = 0; i < data->moves_counter; i++) {
    if (row_no == data->moves[i].coordinates[1]) {
      row_user_moves[row_moves_counter++] = data->moves[i];
    }
  }

  cli_sort_user_moves(row_moves_counter, row_user_moves);

  for (i = 0; i < row_moves_counter; i++) {
    if (row_user_moves[i].type == USER_MOVE_TYPE_SELECT_VALID) {
      switch (row_user_moves[i].user) {
      case User1:
        buffer[buffer_counter++] = 'X';
        break;
      case User2:
        buffer[buffer_counter++] = 'O';
        break;
      default:;
      }
    } else if (row_user_moves[i].type == USER_MOVE_TYPE_SELECT_INVALID) {
      // Add ANSI escape codes for red color
      buffer_counter += snprintf(
          buffer + buffer_counter, buffer_size - buffer_counter,
          "\033[31m%c\033[0m", row_user_moves[i].user == User1 ? 'X' : 'O');
    } else if (row_user_moves[i].type == USER_MOVE_TYPE_HIGHLIGHT) {
      // Add ANSI escape codes for green color
      buffer_counter += snprintf(
          buffer + buffer_counter, buffer_size - buffer_counter,
          "\033[32m%c\033[0m", row_user_moves[i].user == User1 ? 'X' : 'O');
    }
  }

  for (size_t column = 0; column < 3; column++) {
    int found_move = 0;
    for (i = 0; i < row_moves_counter; i++) {
      if (row_user_moves[i].coordinates[0] != column)
        continue;

      found_move = 1;

      if (row_user_moves[i].type == USER_MOVE_TYPE_SELECT_VALID) {
        switch (row_user_moves[i].user) {
        case User1:
          buffer[buffer_counter++] = 'X';
          break;
        case User2:
          buffer[buffer_counter++] = 'O';
          break;
        default:;
        }
      } else if (row_user_moves[i].type == USER_MOVE_TYPE_SELECT_INVALID) {
        // Add ANSI escape codes for red color
        buffer_counter += snprintf(
            buffer + buffer_counter, buffer_size - buffer_counter,
            "\033[31m%c\033[0m", row_user_moves[i].user == User1 ? 'X' : 'O');
      } else if (row_user_moves[i].type == USER_MOVE_TYPE_HIGHLIGHT) {
        // Add ANSI escape codes for green color
        buffer_counter += snprintf(
            buffer + buffer_counter, buffer_size - buffer_counter,
            "\033[32m%c\033[0m", row_user_moves[i].user == User1 ? 'X' : 'O');
      }
    }

    if (!found_move) {
      buffer[buffer_counter++] = ' ';
    }
  }

  // Now moves are prepared to be formated

  return 0;
}

void cli_sort_user_moves(int n, struct UserMove user_moves[n]) {
  // Sort row moves according to column
  size_t sorted_moves_counter = 0;
  struct UserMove tmp_user_move;
  size_t i;

  do {
    for (i = sorted_moves_counter; i < n; i++) {
      if (user_moves[i].coordinates[0] <
          user_moves[sorted_moves_counter].coordinates[0]) {
        tmp_user_move = user_moves[sorted_moves_counter];
        user_moves[sorted_moves_counter] = user_moves[i];
        user_moves[i] = tmp_user_move;
        break;
      }
    }

  } while (sorted_moves_counter++ < n);
}

/* int cli_print_row(struct CliDisplay *display, struct DataToDisplay *data, */
/*                   size_t row_no) { */
/*   size_t buffer_size = 1024; */
/*   char buffer[buffer_size]; */

/*   for (size_t i = 0; i < data->moves_counter; i++) { */
/*     if (data->moves->coordinates[1] == row_no) */
/*   } */

/*   return 0; */
/* } */
