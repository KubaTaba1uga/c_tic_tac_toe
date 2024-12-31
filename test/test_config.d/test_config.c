#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unity.h>

#include "config/config.h"
#include "init/init.h"
#include "utils/logging_utils.h"

// Mock Definitions for Test Variables
#define TEST_VAR_NAME "test_var"
#define TEST_DEFAULT_VALUE "default_value"

struct ConfigOps *config_ops;
struct LoggingUtilsOps *log_ops;

// Test Setup
void setUp() {

  int err;

  config_ops = get_config_ops();
  log_ops = get_logging_utils_ops();

  // Initialize logging and configuration
  err = log_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);
}

// Test Teardown
void tearDown() { log_ops->destroy(); }
/* void tearDown() {} */

// Test Variable Initialization
void test_config_var_init_success() {
  struct ConfigVariable variable;
  int err;

  // Initialize the configuration variable
  err = config_ops->init_var(&variable, TEST_VAR_NAME, TEST_DEFAULT_VALUE);
  TEST_ASSERT_EQUAL_INT(0, err);

  // Verify the variable name and default value
  TEST_ASSERT_EQUAL_STRING(TEST_VAR_NAME, variable.var_name);
  TEST_ASSERT_EQUAL_STRING(TEST_DEFAULT_VALUE, variable.default_value);
}

// Test Variable Addition
void test_config_add_var_success() {
  struct ConfigVariable variable;
  struct ConfigAddVarOutput output;
  int err;

  config_ops->init_var(&variable, TEST_VAR_NAME, TEST_DEFAULT_VALUE);

  err = config_ops->add_var((struct ConfigAddVarInput){.var = &variable},
                            &output);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_GREATER_OR_EQUAL(0, output.var_id);
}

// Test Variable Retrieval by Name
// Test Variable Retrieval by Name
void test_config_get_var_by_name_success() {
  struct ConfigVariable variable;
  struct ConfigAddVarOutput add_output;
  struct ConfigGetVarOutput get_output;
  int err;

  // Initialize and add the variable
  err = config_ops->init_var(&variable, TEST_VAR_NAME, TEST_DEFAULT_VALUE);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops->add_var((struct ConfigAddVarInput){.var = &variable},
                            &add_output);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops->get_var(
      (struct ConfigGetVarInput){.mode = CONFIG_GET_VAR_BY_NAME,
                                 .var_name = TEST_VAR_NAME},
      &get_output);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_STRING(TEST_VAR_NAME, get_output.var_name);
  TEST_ASSERT_EQUAL_STRING(TEST_DEFAULT_VALUE, get_output.value);
}

// Test Variable Retrieval by ID
// Test Variable Retrieval by ID
void test_config_get_var_by_id_success() {
  struct ConfigVariable variable;
  struct ConfigAddVarOutput add_output;
  struct ConfigGetVarOutput get_output;
  int err;

  // Initialize and add the variable
  err = config_ops->init_var(&variable, TEST_VAR_NAME, TEST_DEFAULT_VALUE);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops->add_var((struct ConfigAddVarInput){.var = &variable},
                            &add_output);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = config_ops->get_var(
      (struct ConfigGetVarInput){.mode = CONFIG_GET_VAR_BY_ID,
                                 .var_id = add_output.var_id},
      &get_output);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_STRING(TEST_VAR_NAME, get_output.var_name);
  TEST_ASSERT_EQUAL_STRING(TEST_DEFAULT_VALUE, get_output.value);
}

// Test Variable Retrieval Failure (Invalid Name)
void test_config_get_var_failure_invalid_name() {
  struct ConfigGetVarOutput get_output;
  int err;

  err = config_ops->get_var(
      (struct ConfigGetVarInput){.mode = CONFIG_GET_VAR_BY_NAME,
                                 .var_name = "non_existent_var"},
      &get_output);

  TEST_ASSERT_EQUAL_INT(ENOENT, err);
}

// Test Variable Retrieval Failure (Invalid ID)
void test_config_get_var_failure_invalid_id() {
  struct ConfigGetVarOutput get_output;
  int err;

  // Attempt to retrieve the variable using a compound literal for input
  err = config_ops->get_var(
      (struct ConfigGetVarInput){.mode = CONFIG_GET_VAR_BY_ID, .var_id = -1},
      &get_output);

  // Assert failure due to invalid ID
  TEST_ASSERT_EQUAL_INT(ENOENT, err);
}
