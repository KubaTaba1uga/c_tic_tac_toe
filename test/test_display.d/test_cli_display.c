/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <stddef.h>
#include <string.h>

// Tests framework
#include <unity.h>

// App's internal libs
#include "display/cli.h"
#include "game/game.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "init/init.h"
#include "utils/logging_utils.h"

// Mocks requirement
#include "cli_display_wrapper.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct CliDisplayPrivateOps *cli_display_ops;

/*******************************************************************************
 *    TESTS FRAMEWORK BOILERCODE
 ******************************************************************************/
void setUp() {
  struct InitOps *init_ops = get_init_ops();
  init_game_reg.init = NULL;
  init_game_reg.destroy = NULL;
  init_ops->initialize_system();

  cli_display_ops = get_display_cli_ops()->private_ops;
}

void tearDown() {
  struct InitOps *init_ops = get_init_ops();
  init_ops->destroy_system();
}

/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_cli_sort_user_moves_empty_array(void) {
  struct UserMove user_moves[1] = {}; // Empty array
  cli_display_ops->sort_user_moves(0, user_moves);
  TEST_PASS_MESSAGE("Empty array handled correctly.");
}

void test_cli_sort_user_moves_single_element(void) {
  struct UserMove user_moves[1] = {{.coordinates = {2, 1}}};
  cli_display_ops->sort_user_moves(1, user_moves);
  TEST_ASSERT_EQUAL_INT(2, user_moves[0].coordinates[0]);
  TEST_ASSERT_EQUAL_INT(1, user_moves[0].coordinates[1]);
}

void test_cli_sort_user_moves_sorted_array(void) {
  struct UserMove user_moves[3] = {{.coordinates = {1, 0}},
                                   {.coordinates = {2, 0}},
                                   {.coordinates = {3, 0}}};
  cli_display_ops->sort_user_moves(3, user_moves);
  TEST_ASSERT_EQUAL_INT(1, user_moves[0].coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, user_moves[1].coordinates[0]);
  TEST_ASSERT_EQUAL_INT(3, user_moves[2].coordinates[0]);
}

void test_cli_sort_user_moves_unsorted_array(void) {
  struct UserMove user_moves[3] = {{.coordinates = {3, 0}},
                                   {.coordinates = {1, 0}},
                                   {.coordinates = {2, 0}}};
  cli_display_ops->sort_user_moves(3, user_moves);
  TEST_ASSERT_EQUAL_INT(1, user_moves[0].coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, user_moves[1].coordinates[0]);
  TEST_ASSERT_EQUAL_INT(3, user_moves[2].coordinates[0]);
}

void test_cli_sort_user_moves_with_duplicates(void) {
  struct UserMove user_moves[5] = {{.coordinates = {3, 0}},
                                   {.coordinates = {1, 0}},
                                   {.coordinates = {2, 0}},
                                   {.coordinates = {2, 1}},
                                   {.coordinates = {3, 1}}};
  cli_display_ops->sort_user_moves(5, user_moves);
  TEST_ASSERT_EQUAL_INT(1, user_moves[0].coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, user_moves[1].coordinates[0]);
  TEST_ASSERT_EQUAL_INT(2, user_moves[2].coordinates[0]);
  TEST_ASSERT_EQUAL_INT(3, user_moves[3].coordinates[0]);
  TEST_ASSERT_EQUAL_INT(3, user_moves[4].coordinates[0]);
}
