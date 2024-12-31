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
#include "game/user_move.h"
#include "init/init.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct CliDisplay {
  size_t width;
  size_t height;
};

struct DisplayCliPrivateOps {
  display_display_func_t display;
};

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

  err = display_ops->add_display(
      &(struct DisplayDisplay){.display_name = DISPLAY_CLI_NAME,
                               .display = display_cli_priv_ops->display});
  if (err) {
    return err;
  }

  return 0;
};

static int display_cli_display(struct DisplayData *data) { return 0; };

/*******************************************************************************
 *    MODULARIZATION BOILERCODE
 ******************************************************************************/
struct DisplayCliPrivateOps priv_ops = {
    .display = display_cli_display,
};

struct DisplayCliPrivateOps *get_display_cli_priv_ops(void) {
  return &priv_ops;
}

static struct DisplayCliOps cli_display_ops = {
    .init = display_cli_init,
};

struct DisplayCliOps *get_display_cli_ops(void) { return &cli_display_ops; }
