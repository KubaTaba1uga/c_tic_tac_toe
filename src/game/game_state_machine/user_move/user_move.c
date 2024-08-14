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
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/user_move/user_move.h"
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
static struct UserMove user_move_create(struct UserMoveCreationData);

static char module_id[] = "user_move";
static struct UserMoveStateMachine user_move_state_machine;
struct UserMovePrivateOps user_move_priv_ops = {.set_default_state =
                                                    set_default_state};

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
struct UserMoveOps user_move_ops = {
    .create_move = user_move_create,
};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
struct UserMove user_move_create(struct UserMoveCreationData data) {
  struct UserMoveCoordinates *coordinates;
  struct UserMove new_user_move;
  size_t i;

  coordinates = &user_move_state_machine.state;

  switch (data.input) {
  case INPUT_EVENT_UP:
    coordinates->height = (coordinates->height + 1) % 3;
    new_user_move.type = USER_MOVE_TYPE_HIGHLIGHT;
    break;

  case INPUT_EVENT_DOWN:
    coordinates->height = (coordinates->height + 2) % 3;
    new_user_move.type = USER_MOVE_TYPE_HIGHLIGHT;
    break;

  case INPUT_EVENT_RIGHT:
    coordinates->width = (coordinates->width + 1) % 3;
    new_user_move.type = USER_MOVE_TYPE_HIGHLIGHT;
    break;

  case INPUT_EVENT_LEFT:
    coordinates->width = (coordinates->width + 2) % 3;
    new_user_move.type = USER_MOVE_TYPE_HIGHLIGHT;
    break;

  case INPUT_EVENT_EXIT:
    new_user_move.type = USER_MOVE_TYPE_QUIT;
    break;

  case INPUT_EVENT_SELECT:
    new_user_move.type = USER_MOVE_TYPE_SELECT_VALID;

    for (i = 0; i < data.count; i++) {
      if (data.users_moves[i].type == USER_MOVE_TYPE_SELECT_VALID &&
          data.users_moves[i].coordinates[0] == coordinates->width &&
          data.users_moves[i].coordinates[1] == coordinates->height) {
        new_user_move.type = USER_MOVE_TYPE_SELECT_INVALID;
      }
    }
    break;

  case INPUT_EVENT_NONE:
  case INPUT_EVENT_MAX:
    logging_utils_ops.log_err(module_id, "Invalid input event: %i", data.input);
    new_user_move.type = USER_MOVE_TYPE_SELECT_INVALID;
    break;
  }

  new_user_move.user = data.user;
  new_user_move.coordinates[0] = coordinates->width;
  new_user_move.coordinates[1] = coordinates->height;

  if (new_user_move.type == USER_MOVE_TYPE_SELECT_VALID)
    user_move_priv_ops.set_default_state();

  return new_user_move;
}

int user_move_init(void) {
  user_move_priv_ops.set_default_state();

  return 0;
}

void set_default_state(void) {
  user_move_state_machine.state.height = 1;
  user_move_state_machine.state.width = 1;
}

INIT_REGISTER_SUBSYSTEM_CHILD(&init_user_move_reg, init_game_reg_p);
