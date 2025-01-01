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
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
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
#include "game/user_move.h"
#include "init/init.h"
#include "utils/logging_utils.h"
#include "utils/signals_utils.h"
#include "utils/terminal_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define ANSI_BG_RED "\033[41m"
#define ANSI_RESET_COLOR "\033[0m"
#define ANSI_BG_GRAY "\033[48;5;240m"

struct CliDisplay {
  size_t width;
  size_t height;
};

struct DisplayCliPrivateOps {
  display_display_func_t display;
  int (*configure_terminal)(void);
  void (*restore_terminal)(void);
  int (*find_move)(size_t y, size_t x, struct DisplayData *data,
                   struct UserMove **user_move);
};

static struct SignalUtilsOps *signals_ops;
static struct TerminalUtilsOps *terminal_ops;
static struct DisplayCliPrivateOps *display_cli_priv_ops;
struct DisplayCliPrivateOps *get_display_cli_priv_ops(void);

/*******************************************************************************
 *    API
 ******************************************************************************/
static int display_cli_init(void) {
  struct DisplayOps *display_ops;
  int err;

  display_cli_priv_ops = get_display_cli_priv_ops();
  display_ops = get_display_ops();
  terminal_ops = get_terminal_ops();
  signals_ops = get_signal_utils_ops();

  err = display_ops->add_display(
      &(struct DisplayDisplay){.display_name = DISPLAY_CLI_NAME,
                               .display = display_cli_priv_ops->display});
  if (err) {
    return err;
  }

  signals_ops->add_handler(display_cli_priv_ops->restore_terminal);

  display_cli_priv_ops->configure_terminal();

  return 0;
};

static int display_cli_configure_terminal(void) {
  terminal_ops->disable_echo(STDIN_FILENO);

  return 0;
}

// Function to restore the original terminal settings
static void display_cli_restore_terminal(void) {
  terminal_ops->enable_echo(STDIN_FILENO);
}

static char *display_get_move_string(struct UserMove *move) {
  // Print the move details
  /* printf("Move: Type=%d, User=%d, Coordinates=[%d, %d]\n", move->type, */
  /*        move->user_id, move->coordinates.x, move->coordinates.y); */

  switch (move->type) {
  case USER_MOVE_TYPE_SELECT_INVALID:
    return ANSI_BG_RED " " ANSI_RESET_COLOR;
  case USER_MOVE_TYPE_HIGHLIGHT:
    return ANSI_BG_GRAY " " ANSI_RESET_COLOR;
  case USER_MOVE_TYPE_SELECT_VALID:
    switch (move->user_id) {
    case 0:
      return "x";
    case 1:
      return "o";
    case 2:
      return "#";
    case 3:
      return "$";
    case 4:
      return "%";
    case 5:
      return "+";
    case 6:
      return "?";
    case 7:
      return "V";
    case 8:
      return "H";
    case 9:
      return "f";
    default:
      return "8";
    }
  case USER_MOVE_TYPE_QUIT:
    return "";
  }

  return NULL;
}

static int display_cli_display(struct DisplayData *data) {
  struct UserMove *tmp_user_move;
  char *str_to_display;
  int err;

  for (size_t index_y = 0; index_y < data->board_xy; index_y++) {
    for (size_t index_x = 0; index_x < data->board_xy; index_x++) {
      str_to_display = " ";
      err = display_cli_priv_ops->find_move(index_y, index_x, data,
                                            &tmp_user_move);
      if (err != ENOENT) {
        str_to_display = display_get_move_string(tmp_user_move);
        if (!str_to_display)
          return ENODATA;
      }

      if (index_x != data->board_xy - 1) {
        printf("%s|", str_to_display);
      } else {
        printf("%s\n", str_to_display);
      }
    }
  }

  return 0;
};

static int display_cli_get_move_matching_y_x(size_t y, size_t x,
                                             struct DisplayData *data,
                                             struct UserMove **user_move) {
  const struct UserMove *tmp_user_move;
  for (size_t i = 0; i < data->moves_length; i++) {
    tmp_user_move = &data->moves[i];

    if (tmp_user_move->coordinates.y == y &&
        tmp_user_move->coordinates.x == x) {
      *user_move = (struct UserMove *)tmp_user_move;
      return 0;
    }
  }

  return ENOENT;
};

/*******************************************************************************
 *    MODULARIZATION BOILERCODE
 ******************************************************************************/
struct DisplayCliPrivateOps priv_ops = {
    .display = display_cli_display,
    .configure_terminal = display_cli_configure_terminal,
    .restore_terminal = display_cli_restore_terminal,
    .find_move = display_cli_get_move_matching_y_x,
};

struct DisplayCliPrivateOps *get_display_cli_priv_ops(void) {
  return &priv_ops;
}

static struct DisplayCliOps cli_display_ops = {
    .init = display_cli_init,
};

struct DisplayCliOps *get_display_cli_ops(void) { return &cli_display_ops; }
