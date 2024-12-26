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
static struct InputOps *input_ops;
static struct LoggingUtilsOps *logging_ops;
static struct GameStateMachineState game_sm;
static struct GameSmSubsystemOps *gsm_sub_ops;
static char gsm_module_id[] = "game_state_machine";

static int game_sm_init(void);
static void game_sm_quit(void);
static int validate_input_user(enum Users input_user);
static int validate_input_event(enum InputEvents input_event);
static int game_sm_step(enum InputEvents input_event, enum Users input_user);
static struct GameStateMachineState *game_sm_get_state_machine(void);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameStateMachinePrivOps {
  int (*is_input_user_valid)(enum Users input_user);
  int (*is_input_event_valid)(enum InputEvents input_event);
};

static struct GameStateMachinePrivOps game_sm_priv_ops = {
    .is_input_event_valid = validate_input_event,
    .is_input_user_valid = validate_input_user,
};

struct GameStateMachineOps game_sm_ops = {.step = game_sm_step,
                                          .quit = game_sm_quit,
                                          .get_state_machine =
                                              game_sm_get_state_machine};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
/* struct InitRegistrationData init_game_sm_reg = { */
/*     .id = gsm_module_id, */
/*     .init = game_sm_init, */
/*     .destroy = NULL, */
/* }; */

/*******************************************************************************
 *    API
 ******************************************************************************/
int game_sm_init(void) {
  gsm_sub_ops = get_game_sm_subsystem_ops();
  logging_ops = get_logging_utils_ops();
  input_ops = get_input_ops();

  game_sm.users_moves_count = 0;
  game_sm.current_state = GameStatePlay;
  game_sm.current_user = User1;

  return 0;
}

#include <stdio.h>

void display_game_state(const struct GameStateMachineState *state) {
  if (!state) {
    printf("Invalid state pointer!\n");
    return;
  }

  // Display current state
  printf("Current State: ");
  switch (state->current_state) {
  case GameStatePlay:
    printf("Play\n");
    break;
  case GameStateQuitting:
    printf("Quitting\n");
    break;
  case GameStateQuit:
    printf("Quit\n");
    break;
  case GameStateWin:
    printf("Win\n");
    break;
  default:
    printf("Unknown State\n");
    break;
  }

  // Display current user
  printf("Current User: ");
  switch (state->current_user) {
  case UserNone:
    printf("None\n");
    break;
  case User1:
    printf("User 1 (X)\n");
    break;
  case User2:
    printf("User 2 (O)\n");
    break;
  default:
    printf("Unknown User\n");
    break;
  }

  // Display user moves
  printf("Total Moves: %zu\n", state->users_moves_count);
  for (size_t i = 0; i < state->users_moves_count; i++) {
    printf("Move %zu: User ", i + 1);
    switch (state->users_moves_data[i].user) {
    case User1:
      printf("1 (X)");
      break;
    case User2:
      printf("2 (O)");
      break;
    default:
      printf("Unknown");
      break;
    }
    printf(", Type: ");
    switch (state->users_moves_data[i].type) {
    case USER_MOVE_TYPE_HIGHLIGHT:
      printf("Highlight");
      break;
    case USER_MOVE_TYPE_SELECT_VALID:
      printf("Select Valid");
      break;
    case USER_MOVE_TYPE_SELECT_INVALID:
      printf("Select Invalid");
      break;
    case USER_MOVE_TYPE_QUIT:
      printf("Quit");
      break;
    default:
      printf("Unknown");
      break;
    }
    printf(", Coordinates: (%d, %d)\n",
           state->users_moves_data[i].coordinates[0],
           state->users_moves_data[i].coordinates[1]);
  }
}

int game_sm_step(enum InputEvents input_event, enum Users input_user) {
  struct GameStateMachineInput data;
  int err;

  if (game_sm_priv_ops.is_input_event_valid(input_event) != 0 ||
      game_sm_priv_ops.is_input_user_valid(input_user) != 0)
    return EINVAL;

  logging_ops->log_info(gsm_module_id, "Event %d User %d", input_event,
                        input_user);

  data.input_event = input_event;
  data.input_user = input_user;

  err = gsm_sub_ops->next_state(data, &game_sm);
  if (err) {
    logging_ops->log_err(gsm_module_id,
                         "Quitting the game, because of an error %s.",
                         strerror(err));

    game_sm_ops.quit();

    return err;
  }
  display_game_state(&game_sm);

  return 0;
}

void game_sm_quit(void) { input_ops->stop(); }

struct GameStateMachineState *game_sm_get_state_machine(void) {
  return &game_sm;
};

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
  return 0;
  /* return (input_event <= INPUT_EVENT_NONE) || (input_event >=
   * INPUT_EVENT_MAX); */
}
