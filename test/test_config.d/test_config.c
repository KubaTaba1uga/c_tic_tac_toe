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

// Test Setup
void setUp() {
  int err;

  config_ops = get_config_ops();

  // Initialize logging and configuration
  err = init_logging_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_config_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);
}

// Test Teardown
void tearDown() {}

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
  struct ConfigAddVarInput input;
  struct ConfigAddVarOutput output;
  int err;

  // Initialize the configuration variable
  config_ops->init_var(&variable, TEST_VAR_NAME, TEST_DEFAULT_VALUE);

  // Prepare input for adding the variable
  input.var = &variable;

  // Add the variable to the configuration
  err = config_ops->add_var(input, &output);
  TEST_ASSERT_EQUAL_INT(0, err);

  // Verify the assigned variable ID
  TEST_ASSERT_GREATER_OR_EQUAL(0, output.var_id);
}

// Test Variable Retrieval by Name
void test_config_get_var_by_name_success() {
  struct ConfigVariable variable;
  struct ConfigAddVarInput add_input;
  struct ConfigAddVarOutput add_output;
  struct ConfigGetVarInput get_input;
  struct ConfigGetVarOutput get_output;
  int err;

  // Initialize and add the variable
  config_ops->init_var(&variable, TEST_VAR_NAME, TEST_DEFAULT_VALUE);
  add_input.var = &variable;
  config_ops->add_var(add_input, &add_output);

  // Prepare input for retrieving the variable by name
  get_input.mode = CONFIG_GET_VAR_BY_NAME;
  get_input.var_name = TEST_VAR_NAME;

  // Retrieve the variable
  err = config_ops->get_var(get_input, &get_output);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_STRING(TEST_VAR_NAME, get_output.var_name);
  TEST_ASSERT_EQUAL_STRING(TEST_DEFAULT_VALUE, get_output.value);
}

// Test Variable Retrieval by ID
void test_config_get_var_by_id_success() {
  struct ConfigVariable variable;
  struct ConfigAddVarInput add_input;
  struct ConfigAddVarOutput add_output;
  struct ConfigGetVarInput get_input;
  struct ConfigGetVarOutput get_output;
  int err;

  // Initialize and add the variable
  config_ops->init_var(&variable, TEST_VAR_NAME, TEST_DEFAULT_VALUE);
  add_input.var = &variable;
  config_ops->add_var(add_input, &add_output);

  // Prepare input for retrieving the variable by ID
  get_input.mode = CONFIG_GET_VAR_BY_ID;
  get_input.var_id = add_output.var_id;

  // Retrieve the variable
  err = config_ops->get_var(get_input, &get_output);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_STRING(TEST_VAR_NAME, get_output.var_name);
  TEST_ASSERT_EQUAL_STRING(TEST_DEFAULT_VALUE, get_output.value);
}

// Test Variable Addition Failure (Duplicate Variable)
void test_config_add_var_failure_duplicate() {
  struct ConfigVariable variable;
  struct ConfigAddVarInput input;
  struct ConfigAddVarOutput output;
  int err;

  // Initialize and add the variable once
  config_ops->init_var(&variable, TEST_VAR_NAME, TEST_DEFAULT_VALUE);
  input.var = &variable;
  err = config_ops->add_var(input, &output);
  TEST_ASSERT_EQUAL_INT(0, err);

  // Attempt to add the variable again (should fail)
  err = config_ops->add_var(input, &output);
  TEST_ASSERT_EQUAL_INT(EINVAL, err);
}

// Test Variable Retrieval Failure (Invalid Name)
void test_config_get_var_failure_invalid_name() {
  struct ConfigGetVarInput get_input;
  struct ConfigGetVarOutput get_output;
  int err;

  // Prepare input for invalid variable name
  get_input.mode = CONFIG_GET_VAR_BY_NAME;
  get_input.var_name = "non_existent_var";

  // Attempt to retrieve the variable
  err = config_ops->get_var(get_input, &get_output);
  TEST_ASSERT_EQUAL_INT(ENOENT, err);
}

// Test Variable Retrieval Failure (Invalid ID)
void test_config_get_var_failure_invalid_id() {
  struct ConfigGetVarInput get_input;
  struct ConfigGetVarOutput get_output;
  int err;

  // Prepare input for invalid variable ID
  get_input.mode = CONFIG_GET_VAR_BY_ID;
  get_input.var_id = -1;

  // Attempt to retrieve the variable
  err = config_ops->get_var(get_input, &get_output);
  TEST_ASSERT_EQUAL_INT(ENOENT, err);
}
