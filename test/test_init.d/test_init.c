#include <errno.h>

#include <string.h>
#include <unity.h>

#include "init/init.h"
#include "init_wrapper.h"
#include "utils/logging_utils.h"

static void *game_init_mock;
static struct InitOps *init_ops;
static struct InitPrivateOps *priv_ops;

static int game_init_mock_success(void) { return 0; }
static int game_init_mock_err(void) { return ENOMEM; }
static int init_register_modules_mock(void) {
  struct InitRegistration game_mock_reg = {
      .display_name = "game_mock",
      .init = game_init_mock,
      .destroy = NULL,
  };

  return priv_ops->register_module(game_mock_reg);
};

void setUp() {
  int err;

  priv_ops = get_init_priv_ops();
  init_ops = get_init_ops();

  priv_ops->register_modules = init_register_modules_mock;

  err = init_logging_reg.init();
  TEST_ASSERT_EQUAL(0, err);
}

void tearDown() { init_ops->destroy(); }

void test_init_success(void) {
  int err;

  game_init_mock = game_init_mock_success;

  err = init_ops->initialize();
  TEST_ASSERT_EQUAL_STRING(strerror(0), strerror(err));
}

void test_init_failure(void) {
  int err;

  game_init_mock = game_init_mock_err;

  err = init_ops->initialize();
  TEST_ASSERT_EQUAL_STRING(strerror(ENOMEM), strerror(err));
}
