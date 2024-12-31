// TO-DO modularize std lib utils
/*******************************************************************************
 * @file main.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// App's internal libs
#include "config/config.h"
#include "game/game.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static const char main_id[] = "main";

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

/*******************************************************************************
 *    API
 ******************************************************************************/
#ifdef TEST
int main__(void)
#else
int main(void)
#endif
{
  struct LoggingUtilsOps *logging_ops;
  struct GameOps *game_ops;
  ;
  struct InitOps *init_ops;
  int err;

  logging_ops = get_logging_utils_ops();
  game_ops = get_game_ops();
  init_ops = get_init_ops();

  err = init_ops->initialize();
  if (err) {
    logging_ops->log_err(main_id, "Unable to initialize game: %s.",
                         strerror(err));
    return 1;
  }

  err = game_ops->start();
  if (err) {
    logging_ops->log_err(main_id, "Unable to play the game: %s.",
                         strerror(err));
    return 2;
  }

  logging_ops->log_info(main_id, "Game finished");

  init_ops->destroy();

  return 0;
}
