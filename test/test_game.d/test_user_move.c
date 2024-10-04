#include <string.h>
#include <unity.h>

#include "game/game.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/sub_state_machines/common.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.c"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "input/input.h"
#include "utils/logging_utils.h"

struct LoggingUtilsOps *logging_ops_;

void setUp() {
  logging_ops_ = get_logging_utils_ops();
  logging_ops_->init_loggers();
  user_move_priv_ops.set_default_state();
}

void tearDown() { logging_ops_->destroy_loggers(); }

void test_user_move_reset_state() {
  // Reset state using set_default_state
  user_move_priv_ops.set_default_state();

  // Validate state reset
  TEST_ASSERT_EQUAL_INT(1, user_move_state_machine.state.width);
  TEST_ASSERT_EQUAL_INT(1, user_move_state_machine.state.height);
}

void test_user_move_create_up_higlith() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_UP,
                                        .input_user = User1};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = User1,
                                        .users_moves_count = 0,
                                        .users_moves_data = {}};

  struct UserMove *new_user_move;
  int err;

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates[1]);

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[1]);

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
}

void test_user_move_create_down_higlith() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_DOWN,
                                        .input_user = User1};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = User1,
                                        .users_moves_count = 0,
                                        .users_moves_data = {}};
  struct UserMove *new_user_move;
  int err;

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[1]);

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates[1]);

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
}

void test_user_move_create_left_higlith() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_LEFT,
                                        .input_user = User1};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = User1,
                                        .users_moves_count = 0,
                                        .users_moves_data = {}};
  struct UserMove *new_user_move;
  int err;

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[0]);

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates[0]);

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
}

void test_user_move_create_right_higlith() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_RIGHT,
                                        .input_user = User1};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = User1,
                                        .users_moves_count = 0,
                                        .users_moves_data = {}};
  struct UserMove *new_user_move;
  int err;

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates[0]);

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[0]);

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
}

void test_user_move_create_select() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_SELECT,
                                        .input_user = User1};
  struct GameStateMachineState state = {
      .current_state = GameStatePlay,
      .current_user = User1,
      .users_moves_count = 2,
      .users_moves_data = {{.user = User1,
                            .type = USER_MOVE_TYPE_SELECT_VALID,
                            .coordinates = {0, 0}},
                           {.user = User2,
                            .type = USER_MOVE_TYPE_SELECT_VALID,
                            .coordinates = {1, 0}}}

  };
  struct UserMove *new_user_move;
  int err;

  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_SELECT_VALID, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);

  input.input_event = INPUT_EVENT_DOWN; // Move to 1, 0
  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[1]);

  input.input_event = INPUT_EVENT_SELECT;
  err = user_move_state_machine_next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_SELECT_INVALID, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[1]);
}
