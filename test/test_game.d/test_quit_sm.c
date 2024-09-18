#include <unity.h>

#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/quit_sm_module.c"
#include "game/game_state_machine/user_move/user_move.h"
#include "input/input.h"
#include "utils/logging_utils.h"

#define MAGIC_NUMBER 7337

static int destroy_counter;
static void mock_input_ops_destroy();

void setUp() {
  quit_sm_data.last_user = MAGIC_NUMBER + 1;
  logging_utils_ops.init_loggers();
  input_ops.destroy = mock_input_ops_destroy;
  destroy_counter = 0;
}

void tearDown() { logging_utils_ops.destroy_loggers(); }

void test_quit_sm_get_quitting_state() {
  enum GameStates next_state;
  struct UserMove users_moves[] = {};
  struct UserMove current_move = {.user = GameStatePlay,
                                  .coordinates = {0, 0},
                                  .type = USER_MOVE_TYPE_QUIT};

  struct GameSmNextStateCreationData next_state_data = {
      .current_user_move = current_move,
      .current_state = GameStatePlay,
      .users_moves = users_moves,
      .count = 0};

  next_state = quit_state_machine_next_state(next_state_data);

  TEST_ASSERT_EQUAL_INT(GameStateQuitting, next_state);
  TEST_ASSERT_EQUAL_INT(GameStatePlay, quit_sm_data.last_user);
}

void test_quit_sm_get_quit_state() {
  enum GameStates next_state;
  struct UserMove users_moves[] = {};
  struct UserMove current_move = {.user = GameStatePlay,
                                  .coordinates = {0, 0},
                                  .type = USER_MOVE_TYPE_QUIT};

  struct GameSmNextStateCreationData next_state_data = {
      .current_user_move = current_move,
      .current_state = GameStatePlay,
      .users_moves = users_moves,
      .count = 0};

  next_state = quit_state_machine_next_state(next_state_data);

  TEST_ASSERT_EQUAL_INT(GameStateQuitting, next_state);
  TEST_ASSERT_EQUAL_INT(GameStatePlay, quit_sm_data.last_user);

  next_state_data.current_state = next_state;

  next_state = quit_state_machine_next_state(next_state_data);

  TEST_ASSERT_EQUAL_INT(GameStateQuit, next_state);
  TEST_ASSERT_EQUAL_INT(1, destroy_counter);
}

void test_quit_sm_get_last_user_state() {
  enum GameStates next_state;
  struct UserMove users_moves[] = {};
  struct UserMove current_move = {.user = GameStatePlay,
                                  .coordinates = {0, 0},
                                  .type = USER_MOVE_TYPE_QUIT};

  struct GameSmNextStateCreationData next_state_data = {
      .current_user_move = current_move,
      .current_state = GameStatePlay,
      .users_moves = users_moves,
      .count = 0};

  next_state = quit_state_machine_next_state(next_state_data);

  TEST_ASSERT_EQUAL_INT(GameStateQuitting, next_state);
  TEST_ASSERT_EQUAL_INT(GameStatePlay, quit_sm_data.last_user);

  next_state_data.current_state = next_state;
  next_state_data.current_user_move.type = USER_MOVE_TYPE_SELECT_VALID;

  next_state = quit_state_machine_next_state(next_state_data);

  TEST_ASSERT_EQUAL_INT(GameStatePlay, next_state);
}

void test_quit_sm_get_default_value() {
  enum GameStates next_state;
  struct UserMove users_moves[] = {};
  struct UserMove current_move = {.user = GameStatePlay,
                                  .coordinates = {0, 0},
                                  .type = USER_MOVE_TYPE_SELECT_VALID};

  struct GameSmNextStateCreationData next_state_data = {
      .current_user_move = current_move,
      .current_state = GameStatePlay,
      .users_moves = users_moves,
      .count = 0};

  next_state = quit_state_machine_next_state(next_state_data);

  TEST_ASSERT_EQUAL_INT(GameStatePlay, next_state);
}

void mock_input_ops_destroy() { destroy_counter++; }
