#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <unity.h>

#include "input/keyboard/keyboard.c"
#include "utils/logging_utils.h"

int stdin_backup;
int mockup_callback_counter;
struct timespec ts = {.tv_sec = 0, .tv_nsec = 50000000}; // 50 milliseconds

static int mock_callback(void) {
  mockup_callback_counter++;
  return 0;
}

void setUp() {
  logging_utils_ops.init_loggers();
  mockup_callback_counter = 0;
}

void tearDown() { logging_utils_ops.destroy_loggers(); }

void test_process_stdin() {
  keyboard_ops.initialize();
  logging_utils_ops.log_info("test_process_stdin", "Registering mock callback");
  keyboard_ops.register_callback(mock_callback);

  // Check if the callback was executed
  logging_utils_ops.log_info("test_process_stdin", "Checking callback counter");
  TEST_ASSERT_EQUAL_INT(1, mockup_callback_counter);
}
