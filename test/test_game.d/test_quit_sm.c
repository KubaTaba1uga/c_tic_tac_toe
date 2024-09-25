#include <unity.h>

#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/sub_state_machines/quit_sm_module.c"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "input/input.h"
#include "utils/logging_utils.h"

static int destroy_counter;
static void mock_input_ops_destroy();

void setUp() {
  logging_utils_ops.init_loggers();
  input_ops.destroy = mock_input_ops_destroy;
  destroy_counter = 0;
}

void tearDown() { logging_utils_ops.destroy_loggers(); }

void test_quit_sm_get_quitting_state() {
  struct UserMove current_move = {
      .user = User1, .coordinates = {0, 0}, .type = USER_MOVE_TYPE_QUIT};
  struct GameStateMachineInput input = {.input_user = User1,
                                        .input_event = INPUT_EVENT_EXIT};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = User1,
                                        .users_moves_count = 1,
                                        .users_moves_data = {current_move}};
  int err;

  err = quit_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStateQuitting, state.current_state);
}

void test_quit_sm_get_quit_state() {
  struct UserMove current_move = {
      .user = User1, .coordinates = {0, 0}, .type = USER_MOVE_TYPE_QUIT};
  struct GameStateMachineInput input = {.input_user = User1,
                                        .input_event = INPUT_EVENT_EXIT};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = User1,
                                        .users_moves_count = 1,
                                        .users_moves_data = {current_move}};
  int err;
  err = quit_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStateQuitting, state.current_state);
  TEST_ASSERT_EQUAL_INT(1, destroy_counter);
}

/* void test_quit_sm_get_last_user_state() { */
/*   enum GameStates next_state; */
/*   struct UserMove users_moves[] = {}; */
/*   struct UserMove current_move = {.user = GameStatePlay, */
/*                                   .coordinates = {0, 0}, */
/*                                   .type = USER_MOVE_TYPE_QUIT}; */

/*   struct GameSmNextStateCreationData next_state_data = { */
/*       .current_user_move = current_move, */
/*       .current_state = GameStatePlay, */
/*       .users_moves = users_moves, */
/*       .count = 0}; */

/*   next_state = quit_state_machine_next_state(next_state_data); */

/*   TEST_ASSERT_EQUAL_INT(GameStateQuitting, next_state); */
/*   TEST_ASSERT_EQUAL_INT(GameStatePlay, quit_sm_data.last_user); */

/*   next_state_data.current_state = next_state; */
/*   next_state_data.current_user_move.type = USER_MOVE_TYPE_SELECT_VALID; */

/*   next_state = quit_state_machine_next_state(next_state_data); */

/*   TEST_ASSERT_EQUAL_INT(GameStatePlay, next_state); */
/* } */

/* void test_quit_sm_get_default_value() { */
/*   enum GameStates next_state; */
/*   struct UserMove users_moves[] = {}; */
/*   struct UserMove current_move = {.user = GameStatePlay, */
/*                                   .coordinates = {0, 0}, */
/*                                   .type = USER_MOVE_TYPE_SELECT_VALID}; */

/*   struct GameSmNextStateCreationData next_state_data = { */
/*       .current_user_move = current_move, */
/*       .current_state = GameStatePlay, */
/*       .users_moves = users_moves, */
/*       .count = 0}; */

/*   next_state = quit_state_machine_next_state(next_state_data); */

/*   TEST_ASSERT_EQUAL_INT(GameStatePlay, next_state); */
/* } */

void mock_input_ops_destroy() { destroy_counter++; }
