/*******************************************************************************
 * @file game_state_machine.c
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
#include <stddef.h>
#include <string.h>

// App's internal libs
#include "config/config.h"
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/

struct GameStateMachinePrivOps {
  int (*is_input_user_valid)(enum Users input_user);
  int (*is_input_event_valid)(enum InputEvents input_event);
};

static int game_sm_init(void);
static int validate_input_user(enum Users input_user);
static int validate_input_event(enum InputEvents input_event);
static int game_sm_step(enum InputEvents input_event, enum Users input_user);
static void game_sm_quit(void);

static struct GameStateMachineState game_sm;
static char gsm_module_id[] = "game_state_machine";
static struct GameStateMachinePrivOps game_sm_priv_ops = {
    .is_input_event_valid = validate_input_event,
    .is_input_user_valid = validate_input_user,
};
static struct LoggingUtilsOps *logging_ops;
static struct InputOps *input_ops;

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_game_sm_reg = {
    .id = gsm_module_id,
    .init_func = game_sm_init,
    .destroy_func = NULL,
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct GameStateMachineOps game_sm_ops = {.step = game_sm_step,
                                          .quit = game_sm_quit};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int game_sm_init(void) {
  logging_ops = get_logging_utils_ops();
  input_ops = get_input_ops();
  game_sm.users_moves_count = 0;
  game_sm.current_state = GameStatePlay;
  game_sm.current_user = User1;

  return 0;
}

int game_sm_step(enum InputEvents input_event, enum Users input_user) {
  struct GameStateMachineInput data;
  int err;

  /* logging_ops->log_err(gsm_module_id, "Event %i User %i.", input_event, */
  /* input_user); */

  if (game_sm_priv_ops.is_input_event_valid(input_event) != 0 ||
      game_sm_priv_ops.is_input_user_valid(input_user) != 0)
    return EINVAL;

  /* logging_ops->log_info(gsm_module_id, "Performing step of game state
   * machine"); */

  data.input_event = input_event;
  data.input_user = input_user;

  err = game_sm_subsystem_ops.next_state(data, &game_sm);
  if (err) {
    logging_ops->log_err(gsm_module_id,
                         "Quitting the game, because of an error %s.",
                         strerror(err));

    game_sm_ops.quit();

    return err;
  }

  return 0;
}

void game_sm_quit(void) { input_ops->destroy(); }

int validate_input_user(enum Users input_user) {
  enum Users valid_values[] = {game_sm.current_user, UserNone};
  size_t i;
  for (i = 0; i < sizeof(valid_values) / sizeof(enum Users); i++) {
    if (input_user == valid_values[i])
      return 0;
  }
  return 1;
}

int validate_input_event(enum InputEvents input_event) {
  return (input_event <= INPUT_EVENT_NONE) || (input_event >= INPUT_EVENT_MAX);
}

INIT_REGISTER_SUBSYSTEM_CHILD(&init_game_sm_reg, init_game_reg_p);
