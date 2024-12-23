/*******************************************************************************
 * @file game.c
 * @brief TO-DO
 *
 * TO-DO
 *
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
#include "display/display.h"
#include "game.h"
#include "game/game.h"
#include "game/game_state_machine/game_state_machine.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int game_init(void) {
  struct LoggingUtilsOps *logging_ops;
  struct ConfigOps *config_ops;
  struct InputOps *input_ops;
  int err;

  logging_ops = get_logging_utils_ops();
  config_ops = get_config_ops();
  input_ops = get_input_ops();

  return 0;
}

int game_process(enum InputEvents input_event, int input_registration_id) {
  return game_priv_ops.process(input_event, input_registration_id);
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_game_reg = {
    .id = module_id,
    .init = game_init,
    .destroy = NULL,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GamePrivateOps {
  int (*user1_logic)(enum InputEvents input_event);
  int (*user2_logic)(enum InputEvents input_event);
  int (*logic)(enum InputEvents input_event);
  int (*logic_and_display)(enum InputEvents input_event, enum Users input_user);
};

static struct GamePrivateOps game_priv_ops = {.user1_logic = game_process_user1,
                                              .user2_logic = game_process_user2,
                                              .logic = game_process,
                                              .logic_and_display =
                                                  game_process_and_display

};

static struct GameOps game_ops = {.private_ops = &game_priv_ops};
struct GameOps *get_game_ops(void) { return &game_ops; }
