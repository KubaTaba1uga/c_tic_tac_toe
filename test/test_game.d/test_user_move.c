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
#include "game/game_state_machine/mini_state_machines/common.h"
#include "game/game_state_machine/mini_state_machines/user_move_mini_machine.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

// Mocks requirement
#include "game_sm_user_move_wrapper.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define USER_1_ID 1
#define USER_2_ID 2
static struct LoggingUtilsOps *logging_ops;
static struct GameSmUserMoveModulePrivateOps *user_move_priv_ops;
static struct UserMoveStateMachineState user_move_sm;
static struct GameStateMachineCommonOps *gsm_common_ops;
static struct UserMoveStateMachineState *
mock_user_move_state_machine_get_state(void) {
  return &user_move_sm;
};

/*******************************************************************************
 *    TESTS FRAMEWORK BOILERCODE
 ******************************************************************************/
void setUp() {
  struct InitOps *init_ops;

  init_ops = get_init_ops();
  logging_ops = get_logging_utils_ops();
  user_move_priv_ops = get_user_move_priv_ops();
  gsm_common_ops = get_sm_mini_machines_common_ops();

  // Disable game init and destroy
  init_ops->initialize();

  user_move_priv_ops->get_state = mock_user_move_state_machine_get_state;
  user_move_priv_ops->set_default_state();
}

void tearDown() {
  struct InitOps *init_ops;
  init_ops = get_init_ops();
  init_ops->destroy();
}

/*******************************************************************************
 *    TESTS
 ******************************************************************************/
void test_user_move_reset_state() {
  user_move_priv_ops->set_default_state();

  // Validate state reset
  TEST_ASSERT_EQUAL_INT(1, user_move_sm.coordinates.x);
  TEST_ASSERT_EQUAL_INT(1, user_move_sm.coordinates.y);
}

void test_user_move_create_up_higlith() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_UP,
                                        .device_id = 0};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = USER_1_ID,
                                        .users_moves_offset = 0,
                                        .users_moves = {}};

  struct UserMove *new_user_move;
  int err;

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates.y);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates.y);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.y);
}

void test_user_move_create_down_higlith() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_DOWN,
                                        .device_id = 0};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = USER_1_ID,
                                        .users_moves_offset = 0,
                                        .users_moves = {}};
  struct UserMove *new_user_move;
  int err;

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates.y);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates.y);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.y);
}

void test_user_move_create_left_higlith() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_LEFT,
                                        .device_id = 0};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = USER_1_ID,
                                        .users_moves_offset = 0,
                                        .users_moves = {}};
  struct UserMove *new_user_move;
  int err;

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.y);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates.x);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.y);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates.x);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.y);
}

void test_user_move_create_right_higlith() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_RIGHT,
                                        .device_id = 0};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = USER_1_ID,
                                        .users_moves_offset = 0,
                                        .users_moves = {}};
  struct UserMove *new_user_move;
  int err;

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.y);
  TEST_ASSERT_EQUAL_INT(2, new_user_move->coordinates.x);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.y);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates.x);

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.y);
}

void test_user_move_create_select() {
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_SELECT,
                                        .device_id = 0};
  struct GameStateMachineState state = {
      .current_state = GameStatePlay,
      .current_user = USER_1_ID,
      .users_moves_offset = 2,
      .users_moves = {{.user_id = USER_1_ID,
                       .type = USER_MOVE_TYPE_SELECT_VALID,
                       .coordinates = {0, 0}},
                      {.user_id = USER_2_ID,
                       .type = USER_MOVE_TYPE_SELECT_VALID,
                       .coordinates = {1, 0}}}

  };
  struct UserMove *new_user_move;
  int err;

  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_SELECT_VALID, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.y);

  input.input_event = INPUT_EVENT_DOWN; // Move to 1, 0
  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates.y);

  input.input_event = INPUT_EVENT_SELECT;
  err = user_move_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);

  new_user_move = gsm_common_ops->get_last_move(&state);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_SELECT_INVALID, new_user_move->type);
  TEST_ASSERT_EQUAL_INT(USER_1_ID, new_user_move->user_id);
  TEST_ASSERT_EQUAL_INT(1, new_user_move->coordinates.x);
  TEST_ASSERT_EQUAL_INT(0, new_user_move->coordinates.y);
}
