/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// Tests framework
#include <string.h>
#include <unity.h>

// App's internal libs
#include "game/game.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/sub_state_machines/common.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

// Mocks requirement
#include "game_sm_user_move_wrapper.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static struct LoggingUtilsOps *logging_ops;
static struct GameSmUserMoveModulePrivateOps *user_move_priv_ops;
static struct UserMoveStateMachineState user_move_sm;

static struct UserMoveStateMachineState *
mock_user_move_state_machine_get_state(void) {
  return &user_move_sm;
};

/*******************************************************************************
 *    TESTS FRAMEWORK BOILERCODE
 ******************************************************************************/
void setUp() {
  char *disabled_modules_ids[] = {"game"};
  struct InitOps *init_ops;

  init_ops = get_init_ops();
  logging_ops = get_logging_utils_ops();
  user_move_priv_ops = get_game_sm_user_move_module_ops()->private_ops;

  init_ops->initialize_system_with_disabled_modules(
      sizeof(disabled_modules_ids) / sizeof(char *), disabled_modules_ids);

  user_move_priv_ops->get_state = mock_user_move_state_machine_get_state;
  user_move_priv_ops->set_default_state();
}

void tearDown() {
  struct InitOps *init_ops;
  init_ops = get_init_ops();
  init_ops->destroy_system();
}

/*******************************************************************************
 *    TESTS
 ******************************************************************************/
void test_user_move_reset_state() {
  user_move_priv_ops->set_default_state();

  // Validate state reset
  TEST_ASSERT_EQUAL_INT(1, user_move_sm.coordinates.width);
  TEST_ASSERT_EQUAL_INT(1, user_move_sm.coordinates.height);
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

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates[1]);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[1]);

  err = user_move_priv_ops->next_state(input, &state);

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

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[1]);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates[1]);

  err = user_move_priv_ops->next_state(input, &state);

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

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[0]);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates[0]);

  err = user_move_priv_ops->next_state(input, &state);

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

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates[0]);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[0]);

  err = user_move_priv_ops->next_state(input, &state);

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

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_SELECT_VALID, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[1]);

  input.input_event = INPUT_EVENT_DOWN; // Move to 1, 0
  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[1]);

  input.input_event = INPUT_EVENT_SELECT;
  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops.get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_SELECT_INVALID, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move->user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates[1]);
}
