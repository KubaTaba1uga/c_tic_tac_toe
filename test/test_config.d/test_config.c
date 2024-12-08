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

#define NULL_DEF()                                                             \
  struct ConfigRegistrationData null_def = {                                   \
      .var_name = "non_existing_var_123", .default_value = NULL}

#define VAL_DEF()                                                              \
  struct ConfigRegistrationData val_def = {.var_name = "existing_var_321",     \
                                           .default_value = "super value"}
static struct ArrayUtilsOps *array_ops;
static struct ConfigOps *config_ops;
static struct ConfigPrivateOps *config_priv_ops;
static struct ConfigSubsystem config_subsystem;
struct ConfigSubsystem *mock_config_get_subsystem(void) {
  return &config_subsystem;
};

void setUp() {
  struct InitOps *init_ops;
  int err;
  init_ops = get_init_ops();
  config_ops = get_config_ops();
  array_ops = get_array_utils_ops();
  config_priv_ops = config_ops->private_ops;

  // Disable game init and destroy
  init_game_reg.init = NULL;
  init_game_reg.destroy = NULL;
  init_ops->initialize_system();

  config_priv_ops->get_subsystem = mock_config_get_subsystem;

  err = array_ops->init(&config_subsystem.registrations, max_registrations);
  TEST_ASSERT_EQUAL_INT(0, err);
}

void tearDown() {
  struct InitOps *init_ops = get_init_ops();
  init_ops->destroy_system();
  array_ops->destroy(&config_subsystem.registrations);
}

void test_config_register_var_success(void) {
  int err;
  NULL_DEF();
  VAL_DEF();

  err = config_ops->register_var(&null_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops->register_var(&val_def);

  TEST_ASSERT_EQUAL_INT(0, err);
}

void test_config_register_var_failure(void) {
  size_t i;
  int err;

  NULL_DEF();

  for (i = 0; i < max_registrations; i++) {
    err = config_ops->register_var(&null_def);
  }

  err = config_ops->register_var(&null_def);

  TEST_ASSERT_EQUAL_INT(EINVAL, err);
}

void test_config_get_var_success(void) {
  char *value;
  int err;
  NULL_DEF();
  VAL_DEF();

  err = config_ops->register_var(&null_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops->register_var(&val_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  value = config_ops->get_var("non_existing_var_123");

  TEST_ASSERT_NULL(value);

  value = config_ops->get_var("existing_var_321");

  TEST_ASSERT_NOT_NULL(value);
  TEST_ASSERT_EQUAL_STRING("super value", value);
}

void test_config_get_var_failure(void) {
  char *value;
  int err;
  VAL_DEF();

  err = config_ops->register_var(&val_def);

  TEST_ASSERT_EQUAL_INT(0, err);

  value = config_ops->get_var("non_existing_var_123");

  TEST_ASSERT_NULL(value);
}
