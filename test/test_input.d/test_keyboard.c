// TO-DO test if destruction and init arte working properly.

#include "input/input.h"
#include "input/keyboard/keyboard.c"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard1.c"
#include "utils/logging_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <unity.h>

int stdin_backup;
int mockup_callback_counter;
enum InputEvents input_event;
int pipefd[2];

struct timespec ts = {.tv_sec = 0, .tv_nsec = 50000000};

static void restore_orig_stdin();
static void mock_stdin();

static int mock_keyboard_callback(size_t n, char buffer[n]) {
  logging_utils_ops.log_info("mock_keyboard_callback", "executed");
  mockup_callback_counter++;
  return 0;
}

static int mock_keyboard1_callback(enum InputEvents local_input_event) {
  logging_utils_ops.log_info("mock_keyboard1_callback", "executed %i",
                             local_input_event);
  mockup_callback_counter++;
  input_event = local_input_event;
  return 0;
}

void setUp() {
  logging_utils_ops.init_loggers();
  mockup_callback_counter = 0;
  mock_stdin();
}

void tearDown() {
  restore_orig_stdin();
  logging_utils_ops.destroy_loggers();
}

void test_process_single_stdin() {
  // On low spec machine this test may file due to ts being to small.
  keyboard_ops.initialize();
  keyboard_ops.register_callback(mock_keyboard_callback);

  // Write something to the pipe to simulate stdin input
  write(pipefd[1], "test", strlen("test"));
  close(pipefd[1]);

  thrd_sleep(&ts, NULL);

  keyboard_ops.destroy();

  // Check if the callback was executed
  TEST_ASSERT_EQUAL_INT(1, mockup_callback_counter);
}

void test_process_multiple_stdin() {
  // On low spec machine this test may file due to ts being to small.
  keyboard_ops.initialize();
  keyboard_ops.register_callback(mock_keyboard_callback);

  // Write something to the pipe to simulate stdin input
  write(pipefd[1], "test \n", strlen("test \n"));

  thrd_sleep(&ts, NULL);

  // Write something to the pipe to simulate stdin input
  write(pipefd[1], "test", strlen("test"));

  close(pipefd[1]);

  thrd_sleep(&ts, NULL);

  keyboard_ops.destroy();

  // Check if the callback was executed
  TEST_ASSERT_EQUAL_INT(2, mockup_callback_counter);
}

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

void mock_stdin() {
  // Backup the current stdin
  stdin_backup = dup(STDIN_FILENO);

  // Create a pipe and redirect stdin to it
  pipe(pipefd);
  dup2(pipefd[0], STDIN_FILENO);
}

void restore_orig_stdin() {
  // Restore the original stdin
  dup2(stdin_backup, STDIN_FILENO);
  close(pipefd[0]);
}
