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
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// App's internal libs
#include "config/config.h"
#include "game/game.h"
/* #include "game/game_state_machine/game_sm_subsystem.h" */
#include "game/game_config.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_user.h"
#include "game/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "input/input_common.h"
#include "static_array_lib.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
typedef struct GameStateMachineState GSMState;

SARRS_DECL(GSMState, users_moves, struct UserMove, MAX_USERS_MOVES);

struct GameStateMachinePrivOps {
  int (*validate_device_id)(input_device_id_t device_id);
  int (*validate_input_event)(enum InputEvents input_event);
};

static struct GameOps *game_ops;
static struct InputOps *input_ops;
static struct LoggingUtilsOps *logging_ops;
static struct GameConfigOps *game_config_ops;
static struct GameStateMachineState game_sm;
static char gsm_module_id[] = "game_state_machine";

static struct GameStateMachinePrivOps *gsm_priv_ops;
struct GameStateMachinePrivOps *get_game_state_machine_priv_ops(void);

/*******************************************************************************
 *    API
 ******************************************************************************/

int game_sm_init(void) {

  logging_ops = get_logging_utils_ops();
  input_ops = get_input_ops();
  game_config_ops = get_game_config_ops();
  game_ops = get_game_ops();

  GSMState_users_moves_init(&game_sm);
  game_sm.current_state = GameStatePlay;
  game_sm.current_user = 0;

  return 0;
}

int game_sm_step(enum InputEvents input_event, input_device_id_t device_id) {
  struct GameStateMachineInput data;
  int err;

  logging_ops->log_info(gsm_module_id, "Event %d User %d", input_event,
                        game_sm.current_user);

  err = gsm_priv_ops->validate_input_event(input_event);
  if (err) {
    logging_ops->log_err(gsm_module_id, "Invalid input event: %s",
                         strerror(err));
    return err;
  }

  err = gsm_priv_ops->validate_device_id(device_id);
  if (err) {
    logging_ops->log_err(gsm_module_id, "Invalid device id %d for user %d: %s",
                         device_id, game_sm.current_user, strerror(err));
    return err;
  }

  data.input_event = input_event;
  data.device_id = device_id;

  // Make compiler happy
  (void)data;
  /* err = gsm_sub_ops->next_state(data, &game_sm); */
  /* if (err) { */
  /*   logging_ops->log_err(gsm_module_id, "Unable to get next gsm state: %s",
   */
  /*                        strerror(err)); */

  /*   game_ops->stop(); */

  /*   return err; */
  /* } */

  return 0;
}

static int validate_input_event(enum InputEvents input_event) {
  if ((input_event <= INPUT_EVENT_NONE) || (input_event >= INPUT_EVENT_INVALID))
    return EINVAL;

  return 0;
}

static int validate_device_id(input_device_id_t device_id) {
  struct GameGetUserOutput get_user;
  int err;

  err = game_config_ops->get_user(
      &(struct GameGetUserInput){.user_id = game_sm.current_user}, &get_user);
  if (err) {
    logging_ops->log_err(gsm_module_id, "Unable to get user %d: %s",
                         game_sm.current_user, strerror(err));
    return err;
  }

  if (device_id != get_user.user->device_id) {
    return EINVAL;
  }

  return 0;
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct GameStateMachinePrivOps game_sm_priv_ops = {
    .validate_input_event = validate_input_event,
    .validate_device_id = validate_device_id,
};

struct GameStateMachineOps game_sm_ops = {
    .step = game_sm_step,
};

struct GameStateMachinePrivOps *get_game_state_machine_priv_ops(void) {
  return &game_sm_priv_ops;
}

struct GameStateMachineOps *get_game_state_machine_ops(void) {
  return &game_sm_ops;
}
