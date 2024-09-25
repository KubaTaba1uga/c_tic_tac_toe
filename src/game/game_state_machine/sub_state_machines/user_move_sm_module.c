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

// App's internal libs
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct UserMoveCoordinates {
  int width;
  int height;
};

struct UserMoveStateMachine {
  struct UserMoveCoordinates state;
};

struct UserMovePrivateOps {
  void (*set_default_state)(void);
};

static int user_move_init(void);
static void set_default_state(void);
static int
user_move_state_machine_next_state(struct GameStateMachineInput input,
                                   struct GameStateMachineState *state);
static void
user_move_state_machine_handle_up_event(struct UserMoveCoordinates *coordinates,
                                        struct UserMove *new_user_move);
static void user_move_state_machine_handle_down_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move);
static void user_move_state_machine_handle_left_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move);
static void user_move_state_machine_handle_right_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move);
static void user_move_state_machine_handle_exit_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move);
static void user_move_state_machine_handle_select_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move,
    struct GameStateMachineState *data);

static char module_id[] = "user_move_sm_module";
static struct UserMoveStateMachine user_move_state_machine;
struct UserMovePrivateOps user_move_priv_ops = {.set_default_state =
                                                    set_default_state};
static struct GameSmSubsystemRegistrationData gsm_registration_data = {
    .next_state = user_move_state_machine_next_state,
    .id = module_id,
    .priority = 1 // Always perform first
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_user_move_reg = {
    .id = module_id,
    .init_func = user_move_init,
    .destroy_func = NULL,
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int user_move_state_machine_next_state(struct GameStateMachineInput input,
                                       struct GameStateMachineState *state) {
  struct UserMoveCoordinates *coordinates;
  struct UserMove new_user_move;

  coordinates = &user_move_state_machine.state;
  new_user_move.user = input.input_user;

  switch (input.input_event) {
  case INPUT_EVENT_UP:
    user_move_state_machine_handle_up_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_DOWN:
    user_move_state_machine_handle_down_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_RIGHT:
    user_move_state_machine_handle_right_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_LEFT:
    user_move_state_machine_handle_left_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_EXIT:
    user_move_state_machine_handle_exit_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_SELECT:
    user_move_state_machine_handle_select_event(coordinates, &new_user_move,
                                                state);
    break;

  default:
    logging_utils_ops.log_err(module_id, "Invalid input event %i from %s",
                              input.input_event, input.input_user);
    new_user_move.type = USER_MOVE_TYPE_SELECT_INVALID;
  }

  new_user_move.coordinates[0] = coordinates->width;
  new_user_move.coordinates[1] = coordinates->height;
  state->users_moves_data[state->users_moves_count++] = new_user_move;

  return 0;
}

void user_move_state_machine_handle_up_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move) {
  coordinates->height = (coordinates->height + 1) % 3;
  new_user_move->type = USER_MOVE_TYPE_HIGHLIGHT;
}

void user_move_state_machine_handle_down_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move) {
  coordinates->height = (coordinates->height + 2) % 3;
  new_user_move->type = USER_MOVE_TYPE_HIGHLIGHT;
}

void user_move_state_machine_handle_right_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move) {
  coordinates->width = (coordinates->width + 1) % 3;
  new_user_move->type = USER_MOVE_TYPE_HIGHLIGHT;
}

void user_move_state_machine_handle_left_event(
    struct UserMoveCoordinates *coordinates, struct UserMove *new_user_move) {
  coordinates->width = (coordinates->width + 2) % 3;
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

  for (i = 0; i < data->users_moves_count; i++) {
    if (data->users_moves_data[i].type == USER_MOVE_TYPE_SELECT_VALID &&
        data->users_moves_data[i].coordinates[0] == coordinates->width &&
        data->users_moves_data[i].coordinates[1] == coordinates->height) {
      new_user_move->type = USER_MOVE_TYPE_SELECT_INVALID;
      break;
    }
  }
}

int user_move_init(void) {
  user_move_priv_ops.set_default_state();

  game_sm_subsystem_ops.register_state_machine(&gsm_registration_data);

  return 0;
}

void set_default_state(void) {
  user_move_state_machine.state.height = 1;
  user_move_state_machine.state.width = 1;
}

INIT_REGISTER_SUBSYSTEM_CHILD(&init_user_move_reg, init_game_reg_p);
