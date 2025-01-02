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
#include <errno.h>
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
#include "game/game_state_machine/game_states.h"
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
#define ANSI_BG_YELLOW "\033[43m"

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
  void (*display_player_info)(struct DisplayData *data);
  void (*display_empty_lines)(struct DisplayData *data);
  void (*display_empty_prefix)(struct DisplayData *data);
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

static char *get_user_char(int user_id) {
  switch (user_id) {
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
    return get_user_char(move->user_id);
  case USER_MOVE_TYPE_QUIT:
    return "";
  }

  return NULL;
}

static char *
display_cli_get_move_string_with_invalid_move(struct DisplayData *data,
                                              struct UserMove *user_move,
                                              size_t n, char buffer[n]) {
  const struct UserMove *current_move = &data->moves[data->moves_length - 1];
  char *str_to_disp = display_get_move_string(user_move);
  char *color = "";
  if (current_move->coordinates.y == user_move->coordinates.y &&
      current_move->coordinates.x == user_move->coordinates.x) {
    if (current_move->type == USER_MOVE_TYPE_SELECT_INVALID)
      color = ANSI_BG_RED;
    else if (current_move->type == USER_MOVE_TYPE_HIGHLIGHT)
      color = ANSI_BG_YELLOW;
  }

  snprintf(buffer, n - 1, "%s%s%s", color, str_to_disp, ANSI_RESET_COLOR);

  return buffer;
}

static int display_cli_display(struct DisplayData *data) {
  struct UserMove *tmp_user_move;
  char *str_to_display;
  const size_t buffer_size = 255;
  char buffer[buffer_size];
  int err;

  if (data->game_state == GameStateQuitting) {
    printf("User %i quitting. To quit press q.\n", data->user_id + 1);
    return 0;
  }

  display_cli_priv_ops->display_empty_lines(data);

  display_cli_priv_ops->display_player_info(data);

  for (size_t index_y = 0; index_y < data->board_xy; index_y++) {
    display_cli_priv_ops->display_empty_prefix(data);
    for (size_t index_x = 0; index_x < data->board_xy; index_x++) {
      str_to_display = " ";
      err = display_cli_priv_ops->find_move(index_y, index_x, data,
                                            &tmp_user_move);
      if (err != ENOENT) {
        str_to_display = display_cli_get_move_string_with_invalid_move(
            data, tmp_user_move, buffer_size, buffer);
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

  display_cli_priv_ops->display_empty_lines(data);

  if (data->game_state == GameStateWinning) {
    printf("User %i won. To quit press q.\n", data->user_id + 1);
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

static void display_cli_display_player_info(struct DisplayData *data) {
  printf("Current user: %d=%s\n\n", data->user_id + 1,
         get_user_char(data->user_id));
}

static void display_cli_display_empty_lines(struct DisplayData *data) {
  int err;
  int rows, _;

  err = terminal_ops->get_terminal_dimensions(STDIN_FILENO, &rows, &_);
  if (err) {
    return;
  }

  /* printf("x=%d y=%d\n", cols, rows); */
  for (size_t i = 0; i < (rows - data->board_xy + 1) / 2; i++) {
    printf("\n");
  }
}

static void display_cli_display_empty_prefix(struct DisplayData *data) {
  int err;
  int _, cols;

  err = terminal_ops->get_terminal_dimensions(STDIN_FILENO, &_, &cols);
  if (err) {
    return;
  }

  /* printf("x=%d y=%d\n", cols, rows); */
  for (size_t i = 0; i < (cols - data->board_xy + 2) / 2; i++) {
    printf(" ");
  }
}

/*******************************************************************************
 *    MODULARIZATION BOILERCODE
 ******************************************************************************/
struct DisplayCliPrivateOps priv_ops = {
    .display = display_cli_display,
    .configure_terminal = display_cli_configure_terminal,
    .restore_terminal = display_cli_restore_terminal,
    .find_move = display_cli_get_move_matching_y_x,
    .display_player_info = display_cli_display_player_info,
    .display_empty_lines = display_cli_display_empty_lines,
    .display_empty_prefix = display_cli_display_empty_prefix,
};

struct DisplayCliPrivateOps *get_display_cli_priv_ops(void) {
  return &priv_ops;
}

static struct DisplayCliOps cli_display_ops = {
    .init = display_cli_init,
};

struct DisplayCliOps *get_display_cli_ops(void) { return &cli_display_ops; }
