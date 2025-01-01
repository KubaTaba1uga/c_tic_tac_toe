#include <string.h>
#include <unity.h>

#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_keys_mapping.h"
#include "input/keyboard/keyboard_keys_mapping_1.h"
#include "utils/logging_utils.h"

#include "input_keyboard1_wrapper.h"

static enum InputEvents mock_input_event;
static int mock_callback_counter;
static struct LoggingUtilsOps *log_ops;
static struct InputOps *input_ops;
static struct KeyboardOps *keyboard_ops;
static struct KeyboardKeysMappingOps *keys_mapping_ops;
static struct KeyboardKeysMapping1Ops *keyboard1_ops;
static struct KeyboardKeysMapping1PrivOps *keyboard1_priv_ops;

void setUp(void) {
  int err;

  log_ops = get_logging_utils_ops();
  input_ops = get_input_ops();
  keyboard_ops = get_keyboard_ops();
  keys_mapping_ops = get_keyboard_keys_mapping_ops();
  keyboard1_ops = get_keyboard_keys_mapping_1_ops();
  keyboard1_priv_ops = get_keyboard_keys_mapping_1_priv_ops();

  err = log_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = input_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = keyboard_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = keyboard1_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  mock_callback_counter = 0;
  mock_input_event = INPUT_EVENT_NONE;
}

void tearDown(void) {

  keyboard_ops->stop();

  input_ops->stop();

  log_ops->destroy();
}

void test_keyboard1_event_up(void) {
  char *test_strings[] = {"w", "klkw123w", "klwklwklwkl", "12w12w12w12w12"};
  struct KeyboardKeysMappingCallbackOutput output;

  for (int i = 0; i < sizeof(test_strings) / sizeof(test_strings[0]); i++) {
    struct KeyboardKeysMappingCallbackInput input = {
        .buffer = test_strings[i], .n = strlen(test_strings[i])};

    int err = keyboard1_priv_ops->keyboard_callback(&input, &output);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(INPUT_EVENT_UP, output.input_event);
  }
}

void test_keyboard1_event_down(void) {
  char *test_strings[] = {"s", "12s12s", "21s12s12s", "s12s12s12s12"};
  struct KeyboardKeysMappingCallbackOutput output;

  for (int i = 0; i < sizeof(test_strings) / sizeof(test_strings[0]); i++) {
    struct KeyboardKeysMappingCallbackInput input = {
        .buffer = test_strings[i], .n = strlen(test_strings[i])};

    int err = keyboard1_priv_ops->keyboard_callback(&input, &output);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(INPUT_EVENT_DOWN, output.input_event);
  }
}

void test_keyboard1_event_left(void) {
  char *test_strings[] = {"a", "1a12a", "12a12a12a", "12a12a12a12a12"};
  struct KeyboardKeysMappingCallbackOutput output;

  for (int i = 0; i < sizeof(test_strings) / sizeof(test_strings[0]); i++) {
    struct KeyboardKeysMappingCallbackInput input = {
        .buffer = test_strings[i], .n = strlen(test_strings[i])};

    int err = keyboard1_priv_ops->keyboard_callback(&input, &output);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(INPUT_EVENT_LEFT, output.input_event);
  }
}

void test_keyboard1_event_right(void) {
  char *test_strings[] = {"d", "12d12d", "12d12d12d", "12d12d12d12d12"};
  struct KeyboardKeysMappingCallbackOutput output;

  for (int i = 0; i < sizeof(test_strings) / sizeof(test_strings[0]); i++) {
    struct KeyboardKeysMappingCallbackInput input = {
        .buffer = test_strings[i], .n = strlen(test_strings[i])};

    int err = keyboard1_priv_ops->keyboard_callback(&input, &output);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(INPUT_EVENT_RIGHT, output.input_event);
  }
}
