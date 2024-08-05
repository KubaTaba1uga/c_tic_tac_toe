#include <unity.h>

#include "game/game.h"
#include "game/user_move/user_move.c"
#include "game/user_move/user_move.h"
#include "input/input.h"

extern struct UserMoveStateMachine user_move_state_machine;

void setUp() {
  logging_utils_ops.init_loggers();
  user_move_priv_ops.set_default_state();
}

void tearDown() { logging_utils_ops.destroy_loggers(); }

void test_user_move_reset_state() {
  // Modify state
  user_move_state_machine.state.width = 0;
  user_move_state_machine.state.height = 0;

  // Validate changes
  TEST_ASSERT_EQUAL_INT(0, user_move_state_machine.state.width);
  TEST_ASSERT_EQUAL_INT(0, user_move_state_machine.state.height);

  // Reset state using set_default_state
  user_move_priv_ops.set_default_state();

  // Validate state reset
  TEST_ASSERT_EQUAL_INT(1, user_move_state_machine.state.width);
  TEST_ASSERT_EQUAL_INT(1, user_move_state_machine.state.height);
}

void test_user_move_create_up_higlith() {
  struct UserMove src_users_moves[] = {};

  struct UserMoveCreationData src_user_move_data = {
      .user = User1,
      .count = sizeof(src_users_moves) / sizeof(struct UserMove),
      .input = INPUT_EVENT_UP,
      .users_moves = src_users_moves};
  struct UserMove new_user_move;

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move.coordinates[1]);

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move.coordinates[1]);

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[1]);
}

void test_user_move_create_down_higlith() {
  struct UserMove src_users_moves[] = {};

  struct UserMoveCreationData src_user_move_data = {
      .user = User1,
      .count = sizeof(src_users_moves) / sizeof(struct UserMove),
      .input = INPUT_EVENT_DOWN,
      .users_moves = src_users_moves};
  struct UserMove new_user_move;

  user_move_init();
  TEST_ASSERT_EQUAL_INT(1, user_move_state_machine.state.width);
  TEST_ASSERT_EQUAL_INT(1, user_move_state_machine.state.height);

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move.coordinates[1]);

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move.coordinates[1]);

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[1]);
}

void test_user_move_create_left_higlith() {
  struct UserMove src_users_moves[] = {};

  struct UserMoveCreationData src_user_move_data = {
      .user = User1,
      .count = sizeof(src_users_moves) / sizeof(struct UserMove),
      .input = INPUT_EVENT_LEFT,
      .users_moves = src_users_moves};
  struct UserMove new_user_move;

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[1]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move.coordinates[0]);

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[1]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move.coordinates[0]);

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[1]);
}

void test_user_move_create_right_higlith() {
  struct UserMove src_users_moves[] = {};

  struct UserMoveCreationData src_user_move_data = {
      .user = User1,
      .count = sizeof(src_users_moves) / sizeof(struct UserMove),
      .input = INPUT_EVENT_RIGHT,
      .users_moves = src_users_moves};
  struct UserMove new_user_move;

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[1]);
  TEST_ASSERT_EQUAL_INT(2, new_user_move.coordinates[0]);

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[1]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move.coordinates[0]);

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[1]);
}

void test_user_move_create_select() {
  struct UserMove src_users_moves[2] = {{.user = User1,
                                         .type = USER_MOVE_TYPE_SELECT_VALID,
                                         .coordinates = {0, 0}},
                                        {.user = User2,
                                         .type = USER_MOVE_TYPE_SELECT_VALID,
                                         .coordinates = {1, 0}}};

  struct UserMoveCreationData src_user_move_data = {
      .user = User1,
      .count = sizeof(src_users_moves) / sizeof(struct UserMove),
      .input = INPUT_EVENT_SELECT,
      .users_moves = src_users_moves};
  struct UserMove new_user_move;

  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_SELECT_VALID, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[1]);

  src_user_move_data.input = INPUT_EVENT_DOWN; // Move to 1, 0
  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_HIGHLIGHT, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move.coordinates[1]);

  src_user_move_data.input = INPUT_EVENT_SELECT;
  new_user_move = user_move_ops.create_move(src_user_move_data);

  TEST_ASSERT_EQUAL_INT(USER_MOVE_TYPE_SELECT_INVALID, new_user_move.type);
  TEST_ASSERT_EQUAL_INT(User1, new_user_move.user);
  TEST_ASSERT_EQUAL_INT(1, new_user_move.coordinates[0]);
  TEST_ASSERT_EQUAL_INT(0, new_user_move.coordinates[1]);
}
