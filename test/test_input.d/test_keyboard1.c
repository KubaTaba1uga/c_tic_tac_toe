#include <string.h>

#include <unity.h>

#include "game/game.h"
#include "init/init.h"
#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard1.h"
#include "utils/logging_utils.h"

#include "input_keyboard1_wrapper.h"

enum InputEvents mockup_input_event;
int mockup_callback_counter;
static int mock_keyboard1_callback(enum InputEvents local_input_event);
static struct InputOps *input_ops;
static struct Keyboard1PrivateOps *keyboard1_priv_ops;
static struct LoggingUtilsOps *logging_ops_;

void setUp() {
  struct InitOps *init_ops = get_init_ops();
  // Disable game init and destroy
  init_game_reg.init = NULL;
  init_game_reg.destroy = NULL;
  init_ops->initialize_system();
  logging_ops_ = get_logging_utils_ops();
  input_ops = get_input_ops();
  keyboard1_priv_ops = get_keyboard1_ops()->private_ops;

  mockup_callback_counter = 0;
  mockup_input_event = INPUT_EVENT_NONE;
}

void tearDown() {
  struct InitOps *init_ops = get_init_ops();
  init_ops->destroy_system();
}

void test_keyboard1_logic_up() {
  char test_str[] = "kjhkoow";
  int err;

  input_ops->register_callback("keyboard1", mock_keyboard1_callback);

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_UP, mockup_input_event);
}

void test_keyboard1_logic_down() {
  char test_str[] = "kjhkoowkjhs";
  int err;

  input_ops->register_callback("keyboard1", mock_keyboard1_callback);

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_DOWN, mockup_input_event);
}

void test_keyboard1_logic_left() {
  char test_str[] = "hslklkjaada";
  int err;

  input_ops->register_callback("keyboard1", mock_keyboard1_callback);

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_LEFT, mockup_input_event);
}

void test_keyboard1_logic_right() {
  char test_str[] = "hslklkjaadad";
  int err;

  input_ops->register_callback("keyboard1", mock_keyboard1_callback);

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_RIGHT, mockup_input_event);
}

void test_keyboard1_logic_select() {
  char test_str[] = "jasdh\nopiu";
  int err;

  input_ops->register_callback("keyboard1", mock_keyboard1_callback);

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_SELECT, mockup_input_event);
}

void test_keyboard1_logic_quit() {
  char test_str[] = "jasdh\nopqiu";
  int err;

  input_ops->register_callback("keyboard1", mock_keyboard1_callback);

  err = keyboard1_priv_ops->callback(strlen(test_str), test_str);
  if (err != 0)
    TEST_FAIL_MESSAGE("keyboard1_callback failed");

  TEST_ASSERT_EQUAL_INT(INPUT_EVENT_EXIT, mockup_input_event);
}

int mock_keyboard1_callback(enum InputEvents local_input_event) {
  logging_ops_->log_info("mock_keyboard1_callback", "executed %i",
                         local_input_event);
  mockup_callback_counter++;
  mockup_input_event = local_input_event;
  return 0;
}
