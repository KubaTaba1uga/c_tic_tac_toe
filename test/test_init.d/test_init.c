#include <errno.h>

#include <unity.h>

#include "init/init.h"

#include "init_wrapper.h"

static struct InitOps *init_ops;

int game_init_mock(void) { return ENOMEM; }

void setUp() { init_ops = get_init_ops(); }

void tearDown() { init_ops->destroy(); }

void test_init_success(void) {
  int result = init_ops->initialize();

  TEST_ASSERT_EQUAL(0, result);
}

void test_init_failure(void) {
  struct InitSubsystem test_subsystem;

  struct InitRegistration game_mock_reg = {.data = {
                                               .display_name = "game_mock",
                                               .init = game_init_mock,
                                               .destroy = NULL,
                                           }};

  int reg_result = priv_ops->register_module(&test_subsystem, &game_mock_reg);
  TEST_ASSERT_EQUAL(0, reg_result);

  int result = priv_ops->init_modules(&test_subsystem);
  TEST_ASSERT_EQUAL(ENOMEM, result);

  priv_ops->destroy_modules(&test_subsystem);
  priv_ops->destroy_registrar(&test_subsystem);
}
