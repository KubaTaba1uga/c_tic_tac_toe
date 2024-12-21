#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unity.h>

#include "config/config.h"
#include "init/init.h"
#include "utils/array_utils.h"
#include "utils/logging_utils.h"

#include "config_wrapper.h"
#include "utils/registration_utils.h"

// Mock Definitions for Test Variables
#define TEST_VAR_NAME "test_var"
#define TEST_DEFAULT_VALUE "default_value"
#define MAX_REGISTRATIONS 100

// Mock Data Structures
static struct ArrayUtilsOps *array_ops;
static struct ConfigOps *config_ops;
static struct ConfigPrivateOps *config_priv_ops;

// Test Setup
void setUp() {
  int err;

  config_ops = get_config_ops();
  array_ops = get_array_utils_ops();
  config_priv_ops = get_config_priv_ops();

  // Initialize subsystems
  // Initialize subsystems
  err = init_registration_utils_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_config_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);
}

// Test Teardown
void tearDown() { init_config_reg.destroy(); }

// Test Variable Registration - Success
void test_config_register_var_success() {
  struct ConfigRegistration registration;
  struct ConfigRegisterVarOutput output;
  struct ConfigRegisterVarInput input;
  int err;

  // Initialize the registration
  err = config_ops->registration_init(&registration, TEST_VAR_NAME,
                                      TEST_DEFAULT_VALUE);
  TEST_ASSERT_EQUAL_INT(0, err);

  // Set up the registration input
  input.registration = &registration;

  // Register the variable
  err = config_ops->register_system_var(input, &output);
  TEST_ASSERT_EQUAL_INT(0, err);

  // Verify the registration ID is valid
  TEST_ASSERT_GREATER_OR_EQUAL(0, output.registration_id);
}

// Test Variable Registration - Failure (Invalid Input)
void test_config_register_var_failure_invalid_input() {
  struct ConfigRegisterVarOutput output;
  struct ConfigRegisterVarInput input;
  int err;

  // Set up invalid input
  input.registration = NULL;

  // Attempt to register with invalid input
  err = config_ops->register_system_var(input, &output);
  TEST_ASSERT_EQUAL_INT(EINVAL, err);
}

// Test Variable Registration - Failure (Exceeding Max Registrations)
void test_config_register_var_failure_max_limit() {
  struct ConfigRegistration registration;
  struct ConfigRegisterVarInput input;
  struct ConfigRegisterVarOutput output;
  int err;
  size_t i;

  // Initialize the registration
  config_ops->registration_init(&registration, TEST_VAR_NAME,
                                TEST_DEFAULT_VALUE);

  input.registration = &registration;

  // Register up to the maximum allowed
  for (i = 0; i < MAX_REGISTRATIONS; i++) {
    err = config_ops->register_system_var(input, &output);
    TEST_ASSERT_EQUAL_INT(0, err);
  }

  // Attempt to register one more, exceeding the limit
  err = config_ops->register_system_var(input, &output);
  TEST_ASSERT_EQUAL_INT(ENOBUFS, err);
}

// Test Get Variable - Success
void test_config_get_var_success() {
  struct ConfigRegisterVarOutput reg_output;
  struct ConfigRegisterVarInput reg_input;
  struct ConfigRegistration registration;
  struct ConfigGetVarOutput get_output;
  struct ConfigGetVarInput get_input;

  int err;

  // Initialize and register the variable
  config_ops->registration_init(&registration, TEST_VAR_NAME,
                                TEST_DEFAULT_VALUE);
  reg_input.registration = &registration;
  config_ops->register_system_var(reg_input, &reg_output);

  // Set up retrieval input
  get_input.registration_id = reg_output.registration_id;

  // Retrieve the variable
  err = config_ops->get_system_var(get_input, &get_output);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_STRING(TEST_VAR_NAME, get_output.var_name);
  TEST_ASSERT_EQUAL_STRING(TEST_DEFAULT_VALUE, get_output.value);
}

// Test Get Variable - Failure (Invalid ID)
void test_config_get_var_failure_invalid_id() {
  struct ConfigGetVarInput get_input;
  struct ConfigGetVarOutput get_output;
  int err;

  // Set up retrieval input with invalid ID
  get_input.registration_id = -1;

  // Attempt to retrieve the variable
  err = config_ops->get_system_var(get_input, &get_output);
  TEST_ASSERT_EQUAL_INT(ENOENT, err);
}

// Test Get Variable - Failure (Unset Variable)
void test_config_get_var_failure_unset_variable() {
  struct ConfigRegistration registration;
  struct ConfigRegisterVarInput input;
  struct ConfigRegisterVarOutput output;
  struct ConfigGetVarInput get_input;
  struct ConfigGetVarOutput get_output;
  int err;

  // Initialize and register the variable
  config_ops->registration_init(&registration, TEST_VAR_NAME,
                                TEST_DEFAULT_VALUE);
  input.registration = &registration;
  config_ops->register_system_var(input, &output);

  // Simulate an unset variable by clearing the environment
#ifdef unsetenv
  unsetenv(TEST_VAR_NAME);
#elif defined(putenv)
  putenv("TEST_VAR=");
#elif defined(setenv)
  setenv(TEST_VAR_NAME, "", 1); // Use setenv as a fallback
#else
  // Log or handle the case where environment manipulation isn't supported
  fprintf(stderr, "Environment variable manipulation is not supported\n");
#endif

  // Set up retrieval input
  get_input.registration_id = output.registration_id;

  // Attempt to retrieve the variable
  err = config_ops->get_system_var(get_input, &get_output);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_STRING(TEST_VAR_NAME, get_output.var_name);
  TEST_ASSERT_EQUAL_STRING(TEST_DEFAULT_VALUE, get_output.value);
}
