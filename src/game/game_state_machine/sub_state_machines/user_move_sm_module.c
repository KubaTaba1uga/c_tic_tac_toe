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

struct UserMoveStateMachineState {
  struct UserMoveCoordinates coordinates;
};

static int user_move_state_machine_init(void);
static void user_move_state_machine_set_default_state(void);
static struct UserMoveStateMachineState *
user_move_state_machine_get_state(void);
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

static struct LoggingUtilsOps *logging_ops;
static struct GameSmSubsystemOps *gsm_sub_ops;
static char module_id[] = "user_move_sm_module";
static struct UserMoveStateMachineState user_move_state_machine;
static struct GameSmSubsystemRegistrationData gsm_registration_data = {
    .next_state = user_move_state_machine_next_state,
    .id = module_id,
    .priority = 1 // Always perform first
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
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

struct GameSmUserMoveModulePrivateOps user_move_priv_ops = {
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
    .private_ops = &user_move_priv_ops};

struct GameSmUserMoveModuleOps *get_game_sm_user_move_module_ops(void) {
  return &game_sm_user_move_ops;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_user_move_reg = {
    .id = module_id,
    .init_func = user_move_state_machine_init,
    .destroy_func = NULL,
};

/*******************************************************************************
 *    API
 ******************************************************************************/
int user_move_state_machine_init(void) {
  logging_ops = get_logging_utils_ops();
  gsm_sub_ops = get_game_sm_subsystem_ops();

  user_move_priv_ops.set_default_state();

  gsm_sub_ops->register_state_machine(&gsm_registration_data);

  return 0;
}

struct UserMoveStateMachineState *user_move_state_machine_get_state(void) {
  return &user_move_state_machine;
};

int user_move_state_machine_next_state(struct GameStateMachineInput input,
                                       struct GameStateMachineState *state) {
  struct UserMoveCoordinates *coordinates;
  struct UserMove new_user_move;

  coordinates = &user_move_priv_ops.get_state()->coordinates;
  new_user_move.user = input.input_user;

  switch (input.input_event) {
  case INPUT_EVENT_UP:
    user_move_priv_ops.handle_up_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_DOWN:
    user_move_priv_ops.handle_down_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_RIGHT:
    user_move_priv_ops.handle_right_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_LEFT:
    user_move_priv_ops.handle_left_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_EXIT:
    user_move_priv_ops.handle_exit_event(coordinates, &new_user_move);
    break;

  case INPUT_EVENT_SELECT:
    user_move_priv_ops.handle_select_event(coordinates, &new_user_move, state);
    break;

  default:
    logging_ops->log_err(module_id, "Invalid input event %i from %s",
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

void user_move_state_machine_set_default_state(void) {
  user_move_priv_ops.get_state()->coordinates.height = 1;
  user_move_priv_ops.get_state()->coordinates.width = 1;
}

INIT_REGISTER_SUBSYSTEM_CHILD(&init_user_move_reg, init_game_reg_p);
