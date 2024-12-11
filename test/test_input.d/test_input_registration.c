#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "input/input_common.h"
#include "input/input_registration.h"
#include "utils/std_lib_utils.h"

// Mock function counters
static int mock_wait_counter;
static int mock_stop_counter;
static int mock_start_counter;
static int mock_callback_counter;
static int mock_mallock_failure_toggle;

// Mock function implementations
int mock_wait(void) {
  mock_wait_counter++;
  return 0;
}

int mock_start(void) {
  mock_start_counter++;
  return 0;
}

int mock_stop(void) {
  mock_stop_counter++;
  return 0;
}

int mock_callback(enum InputEvents _) {
  mock_callback_counter++;
  return 0;
}

void *mock_malloc(size_t size) {
  if (mock_mallock_failure_toggle) {
    return NULL;
  }

  return malloc(size);
}

// Test data
static input_reg_t test_reg;

// Setup function
void setUp() {
  mock_wait_counter = 0;
  mock_start_counter = 0;
  mock_stop_counter = 0;
  mock_callback_counter = 0;
  mock_mallock_failure_toggle = 0;

  struct InputRegistrationOps *ops = get_input_reg_ops();
  struct StdLibUtilsOps *std_lib_ops = get_std_lib_utils_ops();

  std_lib_ops->alloc_raw_mem = mock_malloc;

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

  TEST_ASSERT_EQUAL_INT(
      EINVAL, ops->init(NULL, "valid_id", mock_wait, mock_start, mock_stop));
  TEST_ASSERT_EQUAL_INT(
      EINVAL, ops->init(&reg, NULL, mock_wait, mock_start, mock_stop));
  TEST_ASSERT_EQUAL_INT(
      EINVAL, ops->init(&reg, "valid_id", NULL, mock_start, mock_stop));
  TEST_ASSERT_EQUAL_INT(
      EINVAL, ops->init(&reg, "valid_id", mock_wait, NULL, mock_stop));
  TEST_ASSERT_EQUAL_INT(
      EINVAL, ops->init(&reg, "valid_id", mock_wait, mock_start, NULL));
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

// Test wait, start, and stop functions
void test_input_reg_operations() {
  struct InputRegistrationOps *ops = get_input_reg_ops();

  TEST_ASSERT_EQUAL_INT(0, ops->wait(test_reg));
  TEST_ASSERT_EQUAL_INT(1, mock_wait_counter); // Confirm wait was called once

  TEST_ASSERT_EQUAL_INT(0, ops->start(test_reg));
  TEST_ASSERT_EQUAL_INT(1, mock_start_counter); // Confirm start was called once

  TEST_ASSERT_EQUAL_INT(0, ops->stop(test_reg));
  TEST_ASSERT_EQUAL_INT(1, mock_stop_counter); // Confirm stop was called once
}

// Test getters
void test_input_reg_getters() {
  struct InputRegistrationOps *ops = get_input_reg_ops();

  TEST_ASSERT_EQUAL_STRING("test_id", ops->get_id(test_reg));
  TEST_ASSERT_NULL(ops->get_callback(test_reg));
}

// Test setting and getting callback
void test_input_reg_set_get_callback() {
  struct InputRegistrationOps *ops = get_input_reg_ops();

  ops->set_callback(test_reg, mock_callback);
  TEST_ASSERT_EQUAL_PTR(mock_callback, ops->get_callback(test_reg));

  // Simulate callback usage
  ops->get_callback(test_reg)(0);
  TEST_ASSERT_EQUAL_INT(
      1, mock_callback_counter); // Confirm callback was called once
}

// Test memory allocation failure during initialization
void test_input_reg_init_memory_failure() {
  struct InputRegistrationOps *ops = get_input_reg_ops();
  input_reg_t reg;

  // Enable malloc failure
  mock_mallock_failure_toggle = 1;

  // Attempt to initialize
  int result = ops->init(&reg, "test_id", mock_wait, mock_start, mock_stop);
  TEST_ASSERT_EQUAL_INT(ENOMEM, result); // Expect ENOMEM
}
