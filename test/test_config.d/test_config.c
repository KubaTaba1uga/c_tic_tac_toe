#include <errno.h>
#include <string.h>
#include <unity.h>

#include "config/config.c"
#include "config/config.h"

#define NULL_DEF()                                                             \
  struct ConfigRegistrationData null_def = {                                   \
      .var_name = "non_existing_var_123", .default_value = NULL}

#define VAL_DEF()                                                              \
  struct ConfigRegistrationData val_def = {.var_name = "existing_var_321",     \
                                           .default_value = "super value"}

void setUp() {
  config_subsystem.count = 0;
  memset(config_subsystem.registrations, 0,
         sizeof(config_subsystem.registrations));
}

void test_config_register_var_success(void) {
  int err;
  NULL_DEF();
  VAL_DEF();

  err = config_ops.register_var(null_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops.register_var(val_def);

  TEST_ASSERT_EQUAL_INT(0, err);
}

void test_config_register_var_failure(void) {
  int err;
  NULL_DEF();

  config_subsystem.count = MAX_CONFIG_REGISTRATIONS + 1;
  err = config_ops.register_var(null_def);

  TEST_ASSERT_EQUAL_INT(EINVAL, err);
}

void test_config_get_var_success(void) {
  char *value;
  int err;
  NULL_DEF();
  VAL_DEF();

  err = config_ops.register_var(null_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops.register_var(val_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  value = config_ops.get_var("non_existing_var_123");

  TEST_ASSERT_NULL(value);

  value = config_ops.get_var("existing_var_321");

  TEST_ASSERT_NOT_NULL(value);
  TEST_ASSERT_EQUAL_STRING("super value", value);
}

void test_config_get_var_failure(void) {
  char *value;
  int err;
  VAL_DEF();

  err = config_ops.register_var(val_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  value = config_ops.get_var("non_existing_var_123");

  TEST_ASSERT_NULL(value);
}
