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
#include "display/display.h"
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

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct CliDisplayPrivateOps {
  display_display_func_t display;
  int (*init_display)(struct CliDisplay *data);
  int (*get_terminal_size)(struct CliDisplay *data);
};
struct CliDisplayPrivateOps priv_ops = {
    .display = cli_display,
    .get_terminal_size = cli_get_terminal_size,
};

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

int cli_format_row(struct CliDisplay *display, struct DataToDisplay *data,
                   size_t row_no, size_t buffer_size,
                   char buffer[buffer_size]) {

  return 0;
}

int cli_print_row(struct CliDisplay *display, struct DataToDisplay *data,
                  size_t row_no) {
  size_t buffer_size = 1024;
  char buffer[buffer_size];

  for (size_t i = 0; i < data->moves_counter; i++) {
    if (data->moves->coordinates[1] == row_no)
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
