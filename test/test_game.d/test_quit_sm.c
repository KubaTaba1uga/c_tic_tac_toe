#include <unity.h>

#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.c"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/sub_state_machines/quit_sm_module.c"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "input/input.h"
#include "utils/logging_utils.h"

static int destroy_counter;
static void mock_input_ops_destroy();
static struct LoggingUtilsOps *logging_ops_;

void setUp() {
  struct InputOps *input_ops = get_input_ops();
  logging_ops_ = get_logging_utils_ops();
  logging_ops_->init_loggers();
  game_sm_init();
  input_ops->destroy = mock_input_ops_destroy;
  destroy_counter = 0;
}

void tearDown() { logging_ops_->destroy_loggers(); }

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
  TEST_ASSERT_EQUAL_INT(0, destroy_counter);
}

void test_quit_sm_get_quit_state() {
  struct UserMove current_move = {
      .user = User1, .coordinates = {0, 0}, .type = USER_MOVE_TYPE_QUIT};
  struct GameStateMachineInput input = {.input_user = User1,
                                        .input_event = INPUT_EVENT_EXIT};
  struct GameStateMachineState state = {.current_state = GameStateQuitting,
                                        .current_user = User1,
                                        .users_moves_count = 1,
                                        .users_moves_data = {current_move}};
  int err;
  err = quit_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStateQuit, state.current_state);
  TEST_ASSERT_EQUAL_INT(1, destroy_counter);
}

void test_quit_sm_get_play_state() {
  struct UserMove current_move = {.user = User1,
                                  .coordinates = {0, 0},
                                  .type = USER_MOVE_TYPE_SELECT_VALID};
  struct GameStateMachineInput input = {.input_user = User1,
                                        .input_event = INPUT_EVENT_SELECT};
  struct GameStateMachineState state = {.current_state = GameStateQuitting,
                                        .current_user = User1,
                                        .users_moves_count = 1,
                                        .users_moves_data = {current_move}};
  int err;
  err = quit_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStatePlay, state.current_state);
  TEST_ASSERT_EQUAL_INT(0, destroy_counter);
}

void mock_input_ops_destroy() { destroy_counter++; }
