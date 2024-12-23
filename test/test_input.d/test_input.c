#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unity.h>

#include "input/input.h"
#include "input/input_registration.h"
#include "utils/logging_utils.h"
#include "utils/registration_utils.h"

struct InputOps *input_ops;
struct InputRegistrationOps *input_reg_ops;
struct InputRegistration input_reg;

// Mock Counters
static int mock_callback_counter = 0;
static int mock_start_counter = 0;
static int mock_stop_counter = 0;
static int mock_wait_counter = 0;

// Mock Functions
static int mock_callback(enum InputEvents input_event) {
  mock_callback_counter++;
  return 0;
};

static int mock_start() {
  mock_start_counter++;
  return 0;
}

static int mock_stop() {
  mock_stop_counter++;
  return 0;
}

static int mock_wait() {
  mock_wait_counter++;
  return 0;
}

// Test Setup
void setUp() {
  // Initialize the input subsystem
  int err;

  input_ops = get_input_ops();
  input_reg_ops = get_input_reg_ops();
  mock_callback_counter = 0;
  mock_start_counter = 0;
  mock_stop_counter = 0;
  mock_wait_counter = 0;

  err = init_logging_reg.data.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_registration_utils_reg.data.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_input_reg.data.init();
  TEST_ASSERT_EQUAL_INT(0, err);
}

// Test Teardown
void tearDown() {
  // Destroy the input subsystem
  init_input_reg.data.destroy();
}

// Test Module Registration - Success
void test_input_register_module_system_success() {
  struct InputRegisterInput reg_input = {.registration = &input_reg};
  struct InputRegisterOutput reg_output;
  int err;

  err = input_reg_ops->init(&input_reg, "dummy_input", mock_wait, mock_start,
                            mock_stop);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = input_ops->register_module(reg_input, &reg_output);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_GREATER_OR_EQUAL(0, reg_output.registration_id);
}

// Test Module Registration - Failure (Invalid Input)
void test_input_register_module_system_failure_invalid_input() {
  struct InputRegisterInput reg_input = {.registration = NULL, .input = NULL};
  struct InputRegisterOutput reg_output;
  int err;

  err = input_ops->register_module(reg_input, &reg_output);
  TEST_ASSERT_EQUAL_INT(EINVAL, err);
}

// Test Setting Callback - Success
void test_input_set_registration_callback_system_success() {
  struct InputRegisterInput reg_input = {.registration = &input_reg};
  struct InputRegisterOutput reg_output;
  int err;

  err = input_reg_ops->init(&input_reg, "dummy_input", mock_wait, mock_start,
                            mock_stop);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = input_ops->register_module(reg_input, &reg_output);
  TEST_ASSERT_EQUAL_INT(0, err);

  struct InputSetRegistrationCallbackInput callback_input = {
      .callback = mock_callback,
      .registration_id = reg_output.registration_id,
      .input = NULL};
  struct InputSetRegistrationCallbackOutput callback_output;

  err = input_ops->set_callback(callback_input, &callback_output);
  TEST_ASSERT_EQUAL_INT(0, err);
}

// Test Start/Stop/Wait System - Success
void test_input_start_stop_wait_system_success() {
  struct InputRegisterInput reg_input = {.registration = &input_reg};
  struct InputRegisterOutput reg_output;
  int err;

  err = input_reg_ops->init(&input_reg, "dummy_input", mock_wait, mock_start,
                            mock_stop);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = input_ops->register_module(reg_input, &reg_output);
  TEST_ASSERT_EQUAL_INT(0, err);

  struct InputSetRegistrationCallbackInput callback_input = {
      .callback = mock_callback,
      .registration_id = reg_output.registration_id,
      .input = NULL};
  struct InputSetRegistrationCallbackOutput callback_output;

  err = input_ops->set_callback(callback_input, &callback_output);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = input_ops->start();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(1, mock_start_counter);

  err = input_ops->wait();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(1, mock_wait_counter);

  err = input_ops->stop();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(1, mock_stop_counter);
}

void test_input_start_stop_wait_system_failure() {
  struct InputRegisterInput reg_input = {.registration = &input_reg};
  struct InputRegisterOutput reg_output;
  int err;

  err = input_reg_ops->init(&input_reg, "dummy_input", mock_wait, mock_start,
                            mock_stop);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = input_ops->register_module(reg_input, &reg_output);
  TEST_ASSERT_EQUAL_INT(0, err);

  err = input_ops->start();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(0, mock_start_counter);

  err = input_ops->wait();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(0, mock_wait_counter);

  err = input_ops->stop();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(0, mock_stop_counter);
}
