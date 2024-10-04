// TO-DO test if destruction and init are working properly.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <unity.h>

#include "input/input.h"
#include "input/keyboard/keyboard.c"
#include "input/keyboard/keyboard.h"
#include "utils/logging_utils.h"

int stdin_backup;
int mockup_callback_counter;
int pipefd[2];
struct KeyboardOps *keyboard_ops_;
struct LoggingUtilsOps *logging_ops_;

struct timespec ts = {.tv_sec = 0, .tv_nsec = 50000000};

static void restore_orig_stdin();
static void mock_stdin();
static int mock_keyboard_callback(size_t n, char buffer[n]);

void setUp() {
  /* struct InitOps *init_ops = get_init_ops(); */
  mock_stdin();
  /* init_ops->initialize_system(); */

  logging_ops_ = get_logging_utils_ops();
  logging_ops_->init_loggers();
  keyboard_ops_ = get_keyboard_ops();

  mockup_callback_counter = 0;
}

void tearDown() {
  /* struct InitOps *init_ops = get_init_ops(); */
  /* init_ops->destroy_system(); */
  restore_orig_stdin();
  logging_ops_->destroy_loggers();
}

void test_process_single_stdin() {
  // On low spec machine this test may fail due to ts being too small.
  keyboard_ops_->initialize();
  keyboard_ops_->register_callback(mock_keyboard_callback);

  // Write something to the pipe to simulate stdin input
  write(pipefd[1], "test", strlen("test"));
  close(pipefd[1]);

  thrd_sleep(&ts, NULL);

  keyboard_ops_->destroy();

  // Check if the callback was executed
  TEST_ASSERT_EQUAL_INT(1, mockup_callback_counter);
}

void test_process_multiple_stdin() {
  // On low spec machine this test may file due to ts being to small.
  keyboard_ops_->initialize();
  keyboard_ops_->register_callback(mock_keyboard_callback);

  // Write something to the pipe to simulate stdin input
  write(pipefd[1], "test \n", strlen("test \n"));

  thrd_sleep(&ts, NULL);

  // Write something to the pipe to simulate stdin input
  write(pipefd[1], "test", strlen("test"));

  close(pipefd[1]);

  thrd_sleep(&ts, NULL);

  printf("Whatever \n");

  keyboard_ops_->destroy();

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

int mock_keyboard_callback(size_t n, char buffer[n]) {
  /* logging_ops_->log_info("mock_keyboard_callback", "executed"); */
  mockup_callback_counter++;
  return 0;
}
