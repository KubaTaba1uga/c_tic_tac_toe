#include <asm-generic/errno.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>

#include "config/config.h"
#include "game/game.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

// Mock requirement
#include "config_wrapper.h"
#include "utils/subsystem_utils.h"

#define NULL_DEF()                                                             \
  struct ConfigRegistrationData null_def = {                                   \
      .var_name = "non_existing_var_123", .default_value = NULL}

#define VAL_DEF()                                                              \
  struct ConfigRegistrationData val_def = {.var_name = "existing_var_321",     \
                                           .default_value = "super value"}
static struct ArrayUtilsOps *array_ops;
static struct ConfigOps *config_ops;
static struct ConfigPrivateOps *config_priv_ops;

void setUp() {
  int err;
  config_ops = get_config_ops();
  array_ops = get_array_utils_ops();
  config_priv_ops = get_config_priv_ops();

  err = init_subsystem_utils_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_logging_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_config_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);
}

void tearDown() { init_config_reg.destroy(); }

void test_config_register_var_success(void) {
  int err;
  NULL_DEF();
  VAL_DEF();

  err = config_ops->register_system_var(&null_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops->register_system_var(&val_def);

  TEST_ASSERT_EQUAL_INT(0, err);
}

void test_config_register_var_failure(void) {
  size_t i;
  int err;

  NULL_DEF();

  for (i = 0; i < max_registrations; i++) {
    err = config_ops->register_system_var(&null_def);
    TEST_ASSERT_EQUAL_INT(0, err);
  }

  err = config_ops->register_system_var(&null_def);

  TEST_ASSERT_EQUAL_INT(ENOBUFS, err);
}

void test_config_get_var_success(void) {
  char *value;
  int err;
  NULL_DEF();
  VAL_DEF();

  err = config_ops->register_system_var(&null_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops->register_system_var(&val_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  value = config_ops->get_system_var("non_existing_var_123");

  TEST_ASSERT_NULL(value);

  value = config_ops->get_system_var("existing_var_321");

  TEST_ASSERT_NOT_NULL(value);
  TEST_ASSERT_EQUAL_STRING("super value", value);
}

void test_config_get_var_failure(void) {
  char *value;
  int err;
  VAL_DEF();

  err = config_ops->register_system_var(&val_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  value = config_ops->get_system_var("non_existing_var_123");

  TEST_ASSERT_NULL(value);
}
