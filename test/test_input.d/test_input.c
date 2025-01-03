#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unity.h>

#include "init/init.h"
#include "input/input.h"
#include "input/input_common.h"
#include "utils/logging_utils.h"

struct LoggingUtilsOps *log_ops;

// Mock counters for callback and operation tracking
static int mock_callback_counter = 0;
static int mock_start_counter = 0;
static int mock_stop_counter = 0;
static int mock_wait_counter = 0;

// Mock callback and operations
static int mock_callback(enum InputEvents event, input_device_id_t id) {
  mock_callback_counter++;
  return 0;
}

static int mock_start(void) {
  mock_start_counter++;
  return 0;
}

static int mock_stop(void) {
  mock_stop_counter++;
  return 0;
}

static int mock_wait(void) {
  mock_wait_counter++;
  return 0;
}

// Test setup and teardown
void setUp() {
  struct InputOps *input_ops;
  int err;

  log_ops = get_logging_utils_ops();
  input_ops = get_input_ops();

  mock_callback_counter = 0;
  mock_start_counter = 0;
  mock_stop_counter = 0;
  mock_wait_counter = 0;

  err = log_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = input_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);
}

void tearDown() { log_ops->destroy(); }

// Test successful module registration
void test_input_add_device_success() {
  struct InputDevice device = {.wait = mock_wait,
                               .start = mock_start,
                               .stop = mock_stop,
                               .display_name = "Mock Device",
                               .callback = NULL};

  struct InputAddDeviceInput input = {.device = &device};

  struct InputAddDeviceOutput output;
  struct InputOps *ops = get_input_ops();

  int err = ops->add_device(input, &output);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_GREATER_OR_EQUAL(0, output.device_id);
}

// Test failed module registration due to invalid input
void test_input_add_device_failure_invalid_input() {
  struct InputAddDeviceInput input = {.device = NULL};
  struct InputAddDeviceOutput output;

  struct InputOps *ops = get_input_ops();
  int err = ops->add_device(input, &output);

  TEST_ASSERT_EQUAL_INT(EINVAL, err);
}

// Test setting a callback successfully
void test_input_set_registration_callback_success() {
  struct InputDevice device = {.wait = mock_wait,
                               .start = mock_start,
                               .stop = mock_stop,
                               .display_name = "Mock Device",
                               .callback = NULL};

  struct InputAddDeviceInput input = {.device = &device};
  struct InputAddDeviceOutput output;

  struct InputOps *ops = get_input_ops();

  int err = ops->add_device(input, &output);
  TEST_ASSERT_EQUAL_INT(0, err);

  struct InputSetCallbackInput callback_input = {.callback = mock_callback,
                                                 .device_id = output.device_id};

  struct InputSetCallbackOutput callback_output;

  err = ops->set_callback(callback_input, &callback_output);
  TEST_ASSERT_EQUAL_INT(0, err);
}

// Test the start, stop, and wait operations
void test_input_start_stop_wait_operations() {
  struct InputDevice device = {
      .wait = mock_wait,
      .start = mock_start,
      .stop = mock_stop,
      .display_name = "Mock Device",
  };

  struct InputAddDeviceInput input = {.device = &device};
  struct InputAddDeviceOutput output;

  struct InputOps *ops = get_input_ops();

  int err = ops->add_device(input, &output);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = ops->set_callback(
      (struct InputSetCallbackInput){.callback = mock_callback,
                                     .device_id = output.device_id},
      &(struct InputSetCallbackOutput){});
  TEST_ASSERT_EQUAL_INT(0, err);

  err = ops->start();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(1, mock_start_counter);

  err = ops->wait();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(1, mock_wait_counter);

  err = ops->stop();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(1, mock_stop_counter);
}

// Test failed operations due to missing callback
void test_input_start_stop_wait_failure() {
  struct InputDevice device = {.wait = mock_wait,
                               .start = mock_start,
                               .stop = mock_stop,
                               .display_name = "Mock Device",
                               .callback = NULL};

  struct InputAddDeviceInput input = {.device = &device};
  struct InputAddDeviceOutput output;

  struct InputOps *ops = get_input_ops();

  int err = ops->add_device(input, &output);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = ops->start();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(0, mock_start_counter);

  err = ops->wait();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(0, mock_wait_counter);

  err = ops->stop();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(0, mock_stop_counter);
}
