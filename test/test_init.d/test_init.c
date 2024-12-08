#include <errno.h>
#include <unity.h>

#include "game/game.h"
#include "init/init.h"

struct InitOps *init_ops;

int game_init_mock(void) { return ENOMEM; }

void setUp() {
  init_ops = get_init_ops();
  init_game_reg.init = NULL;
  init_game_reg.destroy = NULL;
}

void tearDown() { init_ops->destroy_system(); }

void test_init_success(void) {
  int result = init_ops->initialize_system();

  TEST_ASSERT_EQUAL(0, result);
}

void test_init_failure(void) {
  init_game_reg.init = game_init_mock;

  int result = init_ops->initialize_system();

  TEST_ASSERT_EQUAL(ENOMEM, result);
}
