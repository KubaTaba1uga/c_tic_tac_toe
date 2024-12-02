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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

// App's internal libs
#include "display/cli.h"
#include "display/display.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct CliDisplay {
  size_t width;
  size_t height;
};

static char module_id[] = "display_cli";
static int cli_display(struct DataToDisplay *data);
static int cli_get_terminal_size(struct CliDisplay *data);
static int cli_format_game_row(struct CliDisplay *display,
                               struct DataToDisplay *data, size_t row_no,
                               size_t buffer_size, char buffer[buffer_size]);
static void cli_sort_user_moves(int n, struct UserMove user_moves[n]);

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

struct CliDisplayPrivateOps priv_ops = {.display = cli_display,
                                        .get_terminal_size =
                                            cli_get_terminal_size,
                                        .format_game_row = cli_format_game_row,
                                        .sort_user_moves = cli_sort_user_moves};

static struct DisplayCliOps cli_display_ops = {.private_ops = &priv_ops};

struct DisplayCliOps *get_display_cli_ops(void) { return &cli_display_ops; }

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int cli_display(struct DataToDisplay *data) {
  struct LoggingUtilsOps *logging_ops;
  struct CliDisplay display;
  size_t buffer_size = 1024;
  char buffer[buffer_size];
  int err;

  logging_ops = get_logging_utils_ops();

  err = priv_ops.init_display(&display);
  if (err) {
    logging_ops->log_err(module_id, "Unable to get initialize cli display.");
    return err;
  }

  for (size_t i = 0; i < 3; i++) {
    err = cli_format_game_row(&display, data, i, buffer_size, buffer);
    if (err) {
      logging_ops->log_err(module_id, "Unable to format (%i) cli row.", i);
      return err;
    }
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
  size_t i;

  // Find moves matching the row
  for (i = 0; i < data->moves_counter; i++) {
    if (row_no == data->moves[i].coordinates[1]) {
      row_user_moves[row_moves_counter++] = data->moves[i];
    }
  }

  cli_sort_user_moves(row_moves_counter, row_user_moves);

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
