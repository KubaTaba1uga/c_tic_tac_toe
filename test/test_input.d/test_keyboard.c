#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <unity.h>

#include "input/keyboard/keyboard.c"
#include "input/keyboard/keyboard.h"
#include "utils/logging_utils.h"

int stdin_backup;
int mockup_callback_counter;
int pipefd[2];

struct timespec ts = {.tv_sec = 0, .tv_nsec = 5000};

static void restore_orig_stdin();
static void mock_stdin();

static int mock_callback(void) {
  logging_utils_ops.log_info("mock_callback", "executed");
  mockup_callback_counter++;
  return 0;
}

void setUp() {
  logging_utils_ops.init_loggers();
  mockup_callback_counter = 0;
  mock_stdin();
}

void tearDown() {
  restore_orig_stdin();
  keyboard_ops.destroy();
  logging_utils_ops.destroy_loggers();
}

void test_process_single_stdin() {
  keyboard_ops.initialize();
  keyboard_ops.register_callback(mock_callback);

  // Write something to the pipe to simulate stdin input
  write(pipefd[1], "test", strlen("test"));
  close(pipefd[1]);

  thrd_sleep(&ts, NULL);

  // Check if the callback was executed
  TEST_ASSERT_EQUAL_INT(1, mockup_callback_counter);
}

void test_process_multiple_stdin() {
  keyboard_ops.initialize();
  keyboard_ops.register_callback(mock_callback);

  // Write something to the pipe to simulate stdin input
  write(pipefd[1], "test \n", strlen("test \n"));

  thrd_sleep(&ts, NULL);

  // Write something to the pipe to simulate stdin input
  write(pipefd[1], "test", strlen("test"));

  close(pipefd[1]);

  thrd_sleep(&ts, NULL);

  // Check if the callback was executed
  TEST_ASSERT_EQUAL_INT(2, mockup_callback_counter);
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
