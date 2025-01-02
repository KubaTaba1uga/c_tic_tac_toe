#include <unity.h>

#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/mini_state_machines/win_mini_machine.h"
#include "game/user_move.h"

#include "game_sm_win_wrapper.h"

// Mock data for testing
#define MAX_TEST_MOVES 10

static struct UserMove user_moves[MAX_TEST_MOVES];
static struct UserMove current_user_move;
struct GameSmWinModuleOps *win_ops;
struct GameSmWinModulePrivateOps *win_priv_ops;

void setUp(void) {
  win_ops = get_game_sm_win_module_ops();
  win_priv_ops = get_game_sm_win_module_priv_ops();
}

void tearDown(void) {
  // Clean up after tests
}

void test_process_vertical_win(void) {
  current_user_move.coordinates.x = 0;
  current_user_move.coordinates.y = 0;
  current_user_move.user_id = 1;

  // Add moves vertically aligned with the current move
  for (int i = 0; i < 3; i++) {
    user_moves[i].coordinates.x = 0;
    user_moves[i].coordinates.y = i;
    user_moves[i].user_id = 1;
  }

  bool is_win =
      win_priv_ops->process_vertical_win(&current_user_move, 3, user_moves, 2);

  TEST_ASSERT_TRUE(is_win);
}

void test_process_horizontal_win(void) {
  current_user_move.coordinates.x = 0;
  current_user_move.coordinates.y = 0;
  current_user_move.user_id = 1;

  // Add moves horizontally aligned with the current move
  for (int i = 0; i < 3; i++) {
    user_moves[i].coordinates.x = i;
    user_moves[i].coordinates.y = 0;
    user_moves[i].user_id = 1;
  }

  bool is_win = win_priv_ops->process_horizontal_win(&current_user_move, 3,
                                                     user_moves, 2);

  TEST_ASSERT_TRUE(is_win);
}

void test_process_diagonal_win(void) {
  current_user_move.coordinates.x = 0;
  current_user_move.coordinates.y = 2;
  current_user_move.user_id = 1;

  user_moves[0].coordinates.x = 1;
  user_moves[0].coordinates.y = 1;
  user_moves[0].user_id = 1;

  user_moves[1].coordinates.x = 2;
  user_moves[1].coordinates.y = 0;
  user_moves[1].user_id = 1;

  bool is_win = win_priv_ops->process_diagonal_win_a(&current_user_move, 2,
                                                     user_moves, 2);

  TEST_ASSERT_TRUE(is_win);
}
