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
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "init/init.h"
#include "input/input.h"
#include "input/input_common.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    GLOBALS
 ******************************************************************************/
static struct InputOps *input_ops;
static struct LoggingUtilsOps *logging_ops;
static struct GameSmSubsystemOps *gsm_sub_ops;

/*******************************************************************************
 *    API
 ******************************************************************************/
int game_init(void) {
  input_ops = get_input_ops();
  logging_ops = get_logging_utils_ops();
  gsm_sub_ops = get_game_sm_subsystem_ops();

  return 0;
}

/**
 * @brief Starts the game by entering the main game loop.
 *
 * @return 0 on success, or an error code on failure.
 */
int game_start(void) {
  int err;

  // Start the input subsystem
  logging_ops->log_info(GAME_FILE_NAME, "Starting input subsystem...");
  err = input_ops->start();
  if (err) {
    logging_ops->log_err(GAME_FILE_NAME, "Failed to start input subsystem: %s",
                         strerror(err));
    return err;
  }

  logging_ops->log_info(GAME_FILE_NAME,
                        "Input subsystem started successfully.");

  logging_ops->disable_console_logger();

  err = gsm_sub_ops->display_starting_screen();
  if (err) {
    logging_ops->log_err(
        GAME_FILE_NAME, "Unable to display starting screen: %s", strerror(err));
    return err;
  }

  // Wait for input events
  logging_ops->log_info(GAME_FILE_NAME, "Waiting for input events...");
  err = input_ops->wait();
  if (err) {
    logging_ops->log_err(GAME_FILE_NAME, "Error while waiting for input: %s",
                         strerror(err));
    return err;
  }

  logging_ops->log_info(GAME_FILE_NAME, "Game loop exited successfully.");

  return 0;
}

/**
 * @brief Quits the game, performing necessary cleanup and shutdown.
 */
void game_stop(void) {
  logging_ops = get_logging_utils_ops();

  logging_ops->log_info(GAME_FILE_NAME, "Stopping game...");

  input_ops->stop();

  logging_ops->log_info(GAME_FILE_NAME, "Game stopped successfully.");
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameOps game_ops = {
    .init = game_init,
    .start = game_start,
    .stop = game_stop,
};

struct GameOps *get_game_ops(void) { return &game_ops; }
