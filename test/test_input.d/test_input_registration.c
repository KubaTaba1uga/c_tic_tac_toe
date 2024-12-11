#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "input/input_registration.h"

// Mock function implementations
void mock_wait(void) {}
int mock_start(void) { return 0; }
void mock_stop(void) {}
int mock_callback(void) { return 0; }

// Test data
static input_reg_t test_reg;

// Setup function
void setUp() {
  struct InputRegistrationOps *ops = get_input_reg_ops();
  int result =
      ops->init(&test_reg, "test_id", mock_wait, mock_start, mock_stop);
  TEST_ASSERT_EQUAL_INT(0, result);
}

// Teardown function
void tearDown() {
  struct InputRegistrationOps *ops = get_input_reg_ops();
  ops->destroy(&test_reg);
  TEST_ASSERT_NULL(test_reg);
}

// Test initialization with valid arguments
void test_input_reg_init_success() {
  struct InputRegistrationOps *ops = get_input_reg_ops();
  input_reg_t reg;

  int result = ops->init(&reg, "valid_id", mock_wait, mock_start, mock_stop);
  TEST_ASSERT_EQUAL_INT(0, result);
  TEST_ASSERT_NOT_NULL(reg);
  TEST_ASSERT_EQUAL_STRING("valid_id", ops->get_id(reg));

  ops->destroy(&reg);
}

// Test initialization with invalid arguments
void test_input_reg_init_invalid_args() {
  struct InputRegistrationOps *ops = get_input_reg_ops();
  input_reg_t reg;

  int result = ops->init(NULL, "valid_id", mock_wait, mock_start, mock_stop);
  TEST_ASSERT_EQUAL_INT(EINVAL, result);

  result = ops->init(&reg, NULL, mock_wait, mock_start, mock_stop);
  TEST_ASSERT_EQUAL_INT(EINVAL, result);

  result = ops->init(&reg, "valid_id", NULL, mock_start, mock_stop);
  TEST_ASSERT_EQUAL_INT(EINVAL, result);

  result = ops->init(&reg, "valid_id", mock_wait, NULL, mock_stop);
  TEST_ASSERT_EQUAL_INT(EINVAL, result);

  result = ops->init(&reg, "valid_id", mock_wait, mock_start, NULL);
  TEST_ASSERT_EQUAL_INT(EINVAL, result);
}

// Test destruction
void test_input_reg_destroy() {
  struct InputRegistrationOps *ops = get_input_reg_ops();

  ops->destroy(&test_reg);
  TEST_ASSERT_NULL(test_reg);

  // Destroying already destroyed registration should do nothing
  ops->destroy(&test_reg);
  TEST_ASSERT_NULL(test_reg);
}

// Test getters
void test_input_reg_getters() {
  struct InputRegistrationOps *ops = get_input_reg_ops();

  TEST_ASSERT_EQUAL_STRING("test_id", ops->get_id(test_reg));
  TEST_ASSERT_EQUAL_PTR(mock_wait, ops->get_wait(test_reg));
  TEST_ASSERT_EQUAL_PTR(mock_start, ops->get_start(test_reg));
  TEST_ASSERT_EQUAL_PTR(mock_stop, ops->get_stop(test_reg));
  TEST_ASSERT_NULL(ops->get_callback(test_reg));
}

// Test setting and getting callback
void test_input_reg_set_get_callback() {
  struct InputRegistrationOps *ops = get_input_reg_ops();

  ops->set_callback(test_reg, mock_callback);
  TEST_ASSERT_EQUAL_PTR(mock_callback, ops->get_callback(test_reg));
}

// Test initialization failure due to memory allocation
void test_input_reg_init_memory_failure() {
  // Simulate memory allocation failure
  void *(*original_malloc)(size_t) = malloc;
  malloc = NULL;

  struct InputRegistrationOps *ops = get_input_reg_ops();
  input_reg_t reg;

  int result = ops->init(&reg, "test_id", mock_wait, mock_start, mock_stop);
  TEST_ASSERT_EQUAL_INT(ENOMEM, result);

  malloc = original_malloc; // Restore malloc
}
