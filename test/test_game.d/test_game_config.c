#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <string.h>

#include <unity.h>

#include "config/config.h"
#include "display/cli.h"
#include "display/display.h"
#include "game/game_config.h"
#include "game/game_user.h"
#include "game/user_move.h"
#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_keys_mapping_1.h"
#include "utils/logging_utils.h"

#include "game_config_wrapper.h"

static struct ConfigOps *config_ops;
static struct LoggingUtilsOps *log_ops;
static struct InputOps *input_ops;
static struct GameConfigOps *game_config_ops;
static struct DisplayOps *display_ops;
static struct DisplayCliOps *display_cli_ops;

void setUp(void) {
  struct KeyboardOps *keyboard_ops = get_keyboard_ops();
  struct KeyboardKeysMapping1Ops *km1_ops = get_keyboard_keys_mapping_1_ops();
  config_ops = get_config_ops();
  log_ops = get_logging_utils_ops();
  input_ops = get_input_ops();
  game_config_ops = get_game_config_ops();
  display_ops = get_display_ops();
  display_cli_ops = get_display_cli_ops();

  // Initialize modules
  TEST_ASSERT_EQUAL_INT(0, config_ops->init());
  TEST_ASSERT_EQUAL_INT(0, log_ops->init());
  TEST_ASSERT_EQUAL_INT(0, input_ops->init());
  TEST_ASSERT_EQUAL_INT(0, keyboard_ops->init());
  TEST_ASSERT_EQUAL_INT(0, km1_ops->init());
  TEST_ASSERT_EQUAL_INT(0, display_ops->init());
  TEST_ASSERT_EQUAL_INT(0, display_cli_ops->init());
}

void tearDown(void) {
  // Clean up resources if necessary
}

void test_game_config_initialization(void) {
  // Set environment variable for users_amount
  setenv("users_amount", "5", 1);

  int err = game_config_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  // Verify all users can be retrieved
  for (int i = 0; i < 5; ++i) {
    struct GameGetUserInput user_input = {.user_id = i};
    struct GameGetUserOutput user_output;

    err = game_config_ops->get_user(&user_input, &user_output);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_NOT_NULL(user_output.user);
  }
}

void test_game_config_too_many_users(void) {
  // Attempt to retrieve more users than configured
  setenv("users_amount", "5", 1);

  int err = game_config_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  struct GameGetUserInput user_input = {.user_id = 6}; // Out of bounds
  struct GameGetUserOutput user_output;

  err = game_config_ops->get_user(&user_input, &user_output);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_game_config_unknown_input(void) {
  // Set environment variable for an invalid input device
  setenv("user1_input", "unknown_device", 1);
  setenv("users_amount", "1", 1);

  int err = game_config_ops->init();
  TEST_ASSERT_NOT_EQUAL(0, err);

  // Verify that initialization failed due to the invalid input device
  struct GameGetUserInput user_input = {.user_id = 0};
  struct GameGetUserOutput user_output;

  err = game_config_ops->get_user(&user_input, &user_output);
  TEST_ASSERT_NOT_EQUAL(0, err);
}
