/*******************************************************************************
 * @file game.c
 * @brief Implementation of the core game loop and operations.
 *
 * This file manages the game's lifecycle, including start, stop, and quit
 * operations. It integrates input management and ensures proper shutdown
 * sequences.
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// App's internal libs
#include "config/config.h"
#include "game/game.h"
#include "init/init.h"
#include "input/input.h"
#include "input/input_common.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    GLOBALS
 ******************************************************************************/
static struct InputOps *input_ops;
static struct LoggingUtilsOps *logging_ops;

/*******************************************************************************
 *    API
 ******************************************************************************/
/**
 * @brief Starts the game by entering the main game loop.
 *
 * @return 0 on success, or an error code on failure.
 */
int start_game(void) {
  int err;

  // Fetch subsystem operation handles
  input_ops = get_input_ops();
  logging_ops = get_logging_utils_ops();

  // Start the input subsystem
  logging_ops->log_info(__FILE_NAME__, "Starting input subsystem...");
  err = input_ops->start();
  if (err) {
    logging_ops->log_err(__FILE_NAME__, "Failed to start input subsystem: %s",
                         strerror(err));
    return err;
  }

  logging_ops->log_info(__FILE_NAME__, "Input subsystem started successfully.");

  // Wait for input events
  logging_ops->log_info(__FILE_NAME__, "Waiting for input events...");
  err = input_ops->wait();
  if (err) {
    logging_ops->log_err(__FILE_NAME__, "Error while waiting for input: %s",
                         strerror(err));
    return err;
  }

  logging_ops->log_info(__FILE_NAME__, "Game loop exited successfully.");

  return 0;
}

/**
 * @brief Quits the game, performing necessary cleanup and shutdown.
 */
void quit_game(void) {
  logging_ops = get_logging_utils_ops();

  logging_ops->log_info(__FILE_NAME__, "Quitting game...");

  input_ops->stop();

  logging_ops->log_info(__FILE_NAME__, "Game quit successfully.");
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameOps game_ops = {
    .start = start_game,
    .quit = quit_game,
};

struct GameOps *get_game_ops(void) { return &game_ops; }
