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
#include <string.h>

// App's internal libs
#include "config/config.h"
#include "game/game.h"
#include "game/user_move/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/

#define MAX_GAME_REGISTRATIONS 100

struct GSMSubsystem {
  struct GameStateMachineRegistrationData
      *registrations[MAX_GAME_REGISTRATIONS];
  size_t count;
};

struct GameStateMachinePrivateOps {
  void (*next_state)(struct UserMove user_move);
};

struct GameStateMachine game_state_machine;
static char module_id[] = "game_state_machine";

static int game_state_machine_init(void);
static int game_state_machine_step(enum InputEvents input_event);
static void game_state_machine_next_state(struct UserMove user_move);
static struct GameStateMachinePrivateOps gsm_priv_ops = {
    .next_state = game_state_machine_next_state};
static struct GameSubsystem game_subsystem;

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

int game_state_machine_step(enum InputEvents input_event) {
  if (input_event <= INPUT_EVENT_NONE || input_event >= INPUT_EVENT_MAX)
    return 0;

  gsm_priv_ops.process_state();

  struct UserMove user_move;
  struct UserMoveCreationData user_move_creation_data = {
      .user = game_state_machine.state,
      .input = input_event,
      .count = game_state_machine.count,
      .users_moves = game_state_machine.users_moves};

  user_move = user_move_ops.create_move(user_move_creation_data);

  gsm_priv_ops.next_state(user_move);

  return 0;
}

int game_state_machine_init(void) {

  game_subsystem.count = 0;

  // State 0
  game_state_machine.state = User1;
  game_state_machine.count = 0;

  return 0;
}

void game_state_machine_register_module(
    struct GameStateMachineRegistrationData *gsm_registration_data) {
  if (game_subsystem.count < MAX_GAME_REGISTRATIONS) {
    game_subsystem.registrations[game_subsystem.count++] =
        gsm_registration_data;
  } else {
    logging_utils_ops.log_err(module_id,
                              "Unable to register %s in game state machine, "
                              "no enough space in `registrations` array.",
                              gsm_registration_data->id);
  }
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
void game_state_machine_next_state(struct UserMove user_move) {
  enum GameStates next_state;
  size_t i;

  for (i = 0; i < game_subsystem.count; i++) {
    next_state = game_subsystem.registrations[i]->next_state(
        user_move, game_state_machine.state);

    if (next_state != game_state_machine.state) {
      game_state_machine.state = next_state;
      break;
    }
  }

  if (game_state_machine.state == Resuming) {
    game_state_machine.state = User1;

    for (i = 0; i < game_state_machine.count; i++) {
      if (game_state_machine.users_moves[i].type == USER_MOVE_TYPE_SELECT_VALID)
        game_state_machine.state =
            game_state_machine.users_moves[i].user == User1 ? User2 : User1;
    }
  }
}

void game_state_machine_process_state() {
  switch (game_state_machine.state) {

  case Starting:
    game_state_machine.state = User1;
    break;
  case User1:
  case User2:
    break;

  case Quitting:
    break;
    // Show quiting menu
  }
}
