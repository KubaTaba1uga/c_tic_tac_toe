#include <string.h>
#include <unity.h>

#include "init/init.h"
#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard1.h"
#include "utils/logging_utils.h"
#include "utils/terminal_utils.h"

#include "input_keyboard1_wrapper.h"

enum InputEvents mock_input_event;
int mock_callback_counter;
static struct InputOps *input_ops;
static struct KeyboardOps *keyboard_ops;
static struct LoggingUtilsOps *logging_ops;
static struct TerminalUtilsOps *terminal_ops;
static struct Keyboard1PrivateOps *keyboard1_priv_ops;

static int mock_keyboard1_callback(enum InputEvents local_input_event);

void setUp(void) {
  int err;

  input_ops = get_input_ops();
  keyboard_ops = get_keyboard_ops();
  logging_ops = get_logging_utils_ops();
  terminal_ops = get_terminal_ops();
  keyboard1_priv_ops = get_keyboard1_priv_ops();

  // Initialize required modules
  err = init_logging_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_registration_utils_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_input_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_keyboard_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  mock_callback_counter = 0;
  mock_input_event = INPUT_EVENT_NONE;
}

void tearDown(void) {

  // Destroy initialized modules
  if (init_keyboard_reg.destroy) {
    init_keyboard_reg.destroy();
  }

  if (init_input_reg.destroy) {
    init_input_reg.destroy();
  }

  if (init_registration_utils_reg.destroy) {
    init_registration_utils_reg.destroy();
  }

  if (init_logging_reg.destroy) {
    init_logging_reg.destroy();
  }
}

void test_keyboard1_logic_up(void) {
  char test_str[] = "xyzuw";
  int err;

  err = input_ops->set_callback(
      (struct InputSetRegistrationCallbackInput){
          .callback = mock_keyboard1_callback, .registration_id = 0},
      &(struct InputSetRegistrationCallbackOutput){});
  if (err != 0) {
    TEST_FAIL_MESSAGE("Setting keyboard1_callback failed");
  }

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0) {
    TEST_FAIL_MESSAGE("keyboard1_callback failed");
  }

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_UP, mock_input_event);
}

void test_keyboard1_logic_down(void) {
  char test_str[] = "abcsm";
  int err;

  err = input_ops->set_callback(
      (struct InputSetRegistrationCallbackInput){
          .callback = mock_keyboard1_callback, .registration_id = 0},
      &(struct InputSetRegistrationCallbackOutput){});
  if (err != 0) {
    TEST_FAIL_MESSAGE("Setting keyboard1_callback failed");
  }

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0) {
    TEST_FAIL_MESSAGE("keyboard1_callback failed");
  }

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_DOWN, mock_input_event);
}

void test_keyboard1_logic_left(void) {
  char test_str[] = "nmaadl";
  int err;

  err = input_ops->set_callback(
      (struct InputSetRegistrationCallbackInput){
          .callback = mock_keyboard1_callback, .registration_id = 0},
      &(struct InputSetRegistrationCallbackOutput){});
  if (err != 0) {
    TEST_FAIL_MESSAGE("Setting keyboard1_callback failed");
  }

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0) {
    TEST_FAIL_MESSAGE("keyboard1_callback failed");
  }

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_LEFT, mock_input_event);
}

void test_keyboard1_logic_right(void) {
  char test_str[] = "qwertyd";
  int err;

  err = input_ops->set_callback(
      (struct InputSetRegistrationCallbackInput){
          .callback = mock_keyboard1_callback, .registration_id = 0},
      &(struct InputSetRegistrationCallbackOutput){});
  if (err != 0) {
    TEST_FAIL_MESSAGE("Setting keyboard1_callback failed");
  }

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0) {
    TEST_FAIL_MESSAGE("keyboard1_callback failed");
  }

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_RIGHT, mock_input_event);
}

void test_keyboard1_logic_select(void) {
  char test_str[] = "helloworld\n";
  int err;

  err = input_ops->set_callback(
      (struct InputSetRegistrationCallbackInput){
          .callback = mock_keyboard1_callback, .registration_id = 0},
      &(struct InputSetRegistrationCallbackOutput){});
  if (err != 0) {
    TEST_FAIL_MESSAGE("Setting keyboard1_callback failed");
  }

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0) {
    TEST_FAIL_MESSAGE("keyboard1_callback failed");
  }

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_SELECT, mock_input_event);
}

void test_keyboard1_logic_quit(void) {
  char test_str[] = "goodbyeq";
  int err;

  err = input_ops->set_callback(
      (struct InputSetRegistrationCallbackInput){
          .callback = mock_keyboard1_callback, .registration_id = 0},
      &(struct InputSetRegistrationCallbackOutput){});
  if (err != 0) {
    TEST_FAIL_MESSAGE("Setting keyboard1_callback failed");
  }

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0) {
    TEST_FAIL_MESSAGE("keyboard1_callback failed");
  }

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_EXIT, mock_input_event);
}

int mock_keyboard1_callback(enum InputEvents local_input_event) {
  logging_ops->log_info("mock_keyboard1_callback",
                        "Callback executed for event: %d", local_input_event);
  mock_callback_counter++;
  mock_input_event = local_input_event;
  return 0;
}
