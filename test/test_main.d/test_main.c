#include <unity.h>

#include "game/game.h"
#include "input/input.h"
#include "main.h"

int input_wait_mock(void) { return 0; };

void setUp() {
  struct InputOps *input_ops = get_input_ops();
  /* struct InitOps *init_ops = get_init_ops();   */
  // Disable game init and destroy
  /* init_game_reg.init = NULL; */
  /* init_game_reg.destroy = NULL; */

  input_ops->wait = input_wait_mock;
}

void test_main(void) {
  int result = main__();

  TEST_ASSERT_EQUAL(result, 0);
  TEST_PASS_MESSAGE("main is working.");
}
