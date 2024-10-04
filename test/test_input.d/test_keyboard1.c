#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard1.c"
#include "utils/logging_utils.h"
#include <unity.h>

enum InputEvents input_event;
int mockup_callback_counter;
static int mock_keyboard1_callback(enum InputEvents local_input_event);
struct LoggingUtilsOps *logging_ops_;

void setUp() {
  logging_ops_ = get_logging_utils_ops();
  logging_ops_->init_loggers();
  mockup_callback_counter = 0;
}

void tearDown() { logging_ops_->destroy_loggers(); }

void test_keyboard1_logic_up() {
  char test_str[] = "kjhkoow";
  int err;

  input_keyboard1_reg.callback = mock_keyboard1_callback;
  input_event = INPUT_EVENT_NONE;

  err = keyboard1_callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_UP, input_event);
}

void test_keyboard1_logic_down() {
  char test_str[] = "kjhkoowkjhs";
  int err;

  input_keyboard1_reg.callback = mock_keyboard1_callback;
  input_event = INPUT_EVENT_NONE;

  err = keyboard1_callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_DOWN, input_event);
}

void test_keyboard1_logic_left() {
  char test_str[] = "hslklkjaada";
  int err;

  input_keyboard1_reg.callback = mock_keyboard1_callback;
  input_event = INPUT_EVENT_NONE;

  err = keyboard1_callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_LEFT, input_event);
}

void test_keyboard1_logic_right() {
  char test_str[] = "hslklkjaadad";
  int err;

  input_keyboard1_reg.callback = mock_keyboard1_callback;
  input_event = INPUT_EVENT_NONE;

  err = keyboard1_callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_RIGHT, input_event);
}

void test_keyboard1_logic_select() {
  char test_str[] = "jasdh\nopiu";
  int err;

  input_keyboard1_reg.callback = mock_keyboard1_callback;
  input_event = INPUT_EVENT_NONE;

  err = keyboard1_callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_SELECT, input_event);
}

void test_keyboard1_logic_quit() {
  char test_str[] = "jasdh\nopqiu";
  int err;

  input_keyboard1_reg.callback = mock_keyboard1_callback;
  input_event = INPUT_EVENT_NONE;

  err = keyboard1_callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_EXIT, input_event);
}

int mock_keyboard1_callback(enum InputEvents local_input_event) {
  logging_ops_->log_info("mock_keyboard1_callback", "executed %i",
                         local_input_event);
  mockup_callback_counter++;
  input_event = local_input_event;
  return 0;
}
