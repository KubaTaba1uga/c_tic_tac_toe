/*******************************************************************************
 * @file user_move.c
 * @brief TO-DO
 *
 * TO-DO
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// App's internal libs
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/mini_state_machines/user_move_mini_machine.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct UserMoveStateMachineState {
  struct UserMoveCoordinates coordinates;
};

struct GameSmUserMoveModulePrivateOps {
  int (*init)(void);
  struct UserMoveStateMachineState *(*get_state)(void);
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
  void (*set_default_state)(void);
  void (*handle_up_event)(struct UserMoveCoordinates *coordinates,
                          struct UserMove *new_user_move);
  void (*handle_down_event)(struct UserMoveCoordinates *coordinates,
                            struct UserMove *new_user_move);
  void (*handle_left_event)(struct UserMoveCoordinates *coordinates,
                            struct UserMove *new_user_move);
  void (*handle_right_event)(struct UserMoveCoordinates *coordinates,
                             struct UserMove *new_user_move);
  void (*handle_exit_event)(struct UserMoveCoordinates *coordinates,
                            struct UserMove *new_user_move);
  void (*handle_select_event)(struct UserMoveCoordinates *coordinates,
                              struct UserMove *new_user_move,
                              struct GameStateMachineState *data);
};

static struct LoggingUtilsOps *logging_ops;
static struct GameSmSubsystemOps *gsm_sub_ops;
static char module_id[] = "user_move_sm_module";
static struct UserMoveStateMachineState user_move_state_machine;
static struct GameSmUserMoveModulePrivateOps *user_move_priv_ops;
struct GameSmUserMoveModulePrivateOps *get_user_move_priv_ops(void);

/*******************************************************************************
 *    API
 ******************************************************************************/
int user_move_state_machine_init(void) {
  struct MiniGameStateMachine gsm_mini_machine;
  int err;
  logging_ops = get_logging_utils_ops();
  gsm_sub_ops = get_game_sm_subsystem_ops();
  user_move_priv_ops = get_user_move_priv_ops();

  user_move_priv_ops->set_default_state();

  gsm_mini_machine.next_state = user_move_priv_ops->next_state;
  gsm_mini_machine.display_name = module_id;
  gsm_mini_machine.priority = 2; // always execute second

  err = gsm_sub_ops->add_mini_state_machine(gsm_mini_machine);
  if (err) {
    logging_ops->log_err(module_id, "Unable to add mini state machine: %s",
                         strerror(err));
    return err;
  }

  return 0;
}

struct UserMoveStateMachineState *user_move_state_machine_get_state(void) {
  return &user_move_state_machine;
};

int user_move_state_machine_next_state(struct GameStateMachineInput input,
                                       struct GameStateMachineState *state) {
  struct UserMoveCoordinates *coordinates;
  struct UserMove new_user_move;

  coordinates = &user_move_priv_ops->get_state()->coordinates;
  new_user_move.user_id = state->current_user;

  switch (input.input_event) {
  case INPUT_EVENT_UP:
    user_move_priv_ops->handle_up_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_DOWN:
    user_move_priv_ops->handle_down_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_RIGHT:
    user_move_priv_ops->handle_right_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_LEFT:
    user_move_priv_ops->handle_left_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_EXIT:
    user_move_priv_ops->handle_exit_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_SELECT:
    user_move_priv_ops->handle_select_event(coordinates, &new_user_move, state);
    break;

  default:
    logging_ops->log_err(module_id, "Invalid input event %i from user%i",
                         input.input_event, state->current_user);
    new_user_move.type = USER_MOVE_TYPE_SELECT_INVALID;
  }

  new_user_move.coordinates.x = coordinates->x;
  new_user_move.coordinates.y = coordinates->y;

  state->users_moves[state->users_moves_offset++] = new_user_move;

  return 0;
}

void user_move_state_machine_handle_up_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move) {
  coordinates->y = (coordinates->y + 1) % 3;
  new_user_move->type = USER_MOVE_TYPE_HIGHLIGHT;
}

void user_move_state_machine_handle_down_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move) {
  coordinates->y = (coordinates->y + 2) % 3;
  new_user_move->type = USER_MOVE_TYPE_HIGHLIGHT;
}

void user_move_state_machine_handle_right_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move) {
  coordinates->x = (coordinates->x + 1) % 3;
  new_user_move->type = USER_MOVE_TYPE_HIGHLIGHT;
}

void user_move_state_machine_handle_left_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move) {
  coordinates->x = (coordinates->x + 2) % 3;
  new_user_move->type = USER_MOVE_TYPE_HIGHLIGHT;
}

void user_move_state_machine_handle_exit_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move) {
  (void)coordinates;
  new_user_move->type = USER_MOVE_TYPE_QUIT;
}

void user_move_state_machine_handle_select_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move,
    struct GameStateMachineState *data) {
  size_t i;

  new_user_move->type = USER_MOVE_TYPE_SELECT_VALID;

  for (i = 0; i < data->users_moves_offset; i++) {
    if (data->users_moves[i].type == USER_MOVE_TYPE_SELECT_VALID &&
        data->users_moves[i].coordinates.x == coordinates->x &&
        data->users_moves[i].coordinates.y == coordinates->y) {
      new_user_move->type = USER_MOVE_TYPE_SELECT_INVALID;
      break;
    }
  }
}

void user_move_state_machine_set_default_state(void) {
  user_move_priv_ops->get_state()->coordinates.y = 1;
  user_move_priv_ops->get_state()->coordinates.x = 1;
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmUserMoveModulePrivateOps user_move_priv_ops_ = {
    .init = user_move_state_machine_init,
    .next_state = user_move_state_machine_next_state,
    .get_state = user_move_state_machine_get_state,
    .set_default_state = user_move_state_machine_set_default_state,
    .handle_up_event = user_move_state_machine_handle_up_event,
    .handle_down_event = user_move_state_machine_handle_down_event,
    .handle_left_event = user_move_state_machine_handle_left_event,
    .handle_right_event = user_move_state_machine_handle_right_event,
    .handle_exit_event = user_move_state_machine_handle_exit_event,
    .handle_select_event = user_move_state_machine_handle_select_event};

static struct GameSmUserMoveModuleOps game_sm_user_move_ops = {
    .init = user_move_state_machine_init};

struct GameSmUserMoveModuleOps *get_game_sm_user_move_module_ops(void) {
  return &game_sm_user_move_ops;
}

struct GameSmUserMoveModulePrivateOps *get_user_move_priv_ops(void) {
  return &user_move_priv_ops_;
}
