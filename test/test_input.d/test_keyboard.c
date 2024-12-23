#include <pty.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <unity.h>

#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_registration.h"
#include "utils/logging_utils.h"
#include "utils/terminal_utils.h"

static int stdin_backup;
static int master_fd; // PTY master file descriptor
static int mock_callback_counter = 0;
static struct KeyboardOps *keyboard_ops;
static struct LoggingUtilsOps *logging_ops;
static struct TerminalUtilsOps *terminal_ops;
static struct KeyboardRegistration keyboard_registration;

struct timespec ts = {.tv_sec = 0, .tv_nsec = 50000000};

static void restore_original_stdin(void);
static void setup_pty(void);
static int mock_callback(size_t n, char buffer[n]);

void setUp(void) {
  int err;

  keyboard_ops = get_keyboard_ops();
  logging_ops = get_logging_utils_ops();
  terminal_ops = get_terminal_ops();

  err = init_logging_reg.data.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_registration_utils_reg.data.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_input_reg.data.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = init_keyboard_reg.data.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  setup_pty();

  // Initialize keyboard registration for tests
  struct KeyboardRegistrationOps *reg_ops = get_keyboard_registration_ops();
  TEST_ASSERT_EQUAL_INT(
      0, reg_ops->init(&keyboard_registration, "test_keyboard", mock_callback));

  mock_callback_counter = 0; // Reset the callback counter
}

void tearDown(void) { restore_original_stdin(); }

void test_keyboard_single_input(void) {
  keyboard_ops->register_callback(
      (struct KeyboardRegisterInput){.registration = &keyboard_registration},
      &(struct KeyboardRegisterOutput){});
  keyboard_ops->start();

  // Simulate a single character input
  write(master_fd, "a", 1);

  thrd_sleep(&ts, NULL); // Allow thread to process input

  keyboard_ops->stop();

  TEST_ASSERT_EQUAL_INT(1, mock_callback_counter);
}

void test_keyboard_multiple_inputs(void) {
  keyboard_ops->register_callback(
      (struct KeyboardRegisterInput){.registration = &keyboard_registration},
      &(struct KeyboardRegisterOutput){});
  keyboard_ops->start();

  // Simulate multiple inputs
  const char *inputs[] = {"abc", "xyz", "123"};
  for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    write(master_fd, inputs[i], strlen(inputs[i]));
    thrd_sleep(&ts, NULL); // Allow thread to process input
    TEST_ASSERT_EQUAL_INT(i + 1, mock_callback_counter);
  }

  keyboard_ops->stop();
}

void test_keyboard_thread_restart(void) {
  keyboard_ops->register_callback(
      (struct KeyboardRegisterInput){.registration = &keyboard_registration},
      &(struct KeyboardRegisterOutput){});
  keyboard_ops->start();

  // Simulate input and stop the thread
  write(master_fd, "x", 1);
  thrd_sleep(&ts, NULL);

  keyboard_ops->stop();
  TEST_ASSERT_EQUAL_INT(1, mock_callback_counter);

  // Restart the thread and simulate new input
  keyboard_ops->start();
  write(master_fd, "y", 1);
  thrd_sleep(&ts, NULL);

  keyboard_ops->stop();
  TEST_ASSERT_EQUAL_INT(2, mock_callback_counter);
}

void setup_pty(void) {
  int slave_fd;

  stdin_backup = dup(STDIN_FILENO);

  if (openpty(&master_fd, &slave_fd, NULL, NULL, NULL) == -1) {
    perror("openpty");
    exit(EXIT_FAILURE);
  }

  dup2(slave_fd, STDIN_FILENO);
  close(slave_fd);

  terminal_ops->disable_canonical_mode(master_fd);
}

void restore_original_stdin(void) {
  dup2(stdin_backup, STDIN_FILENO);
  close(stdin_backup);
  close(master_fd);
}

int mock_callback(size_t n, char buffer[n]) {
  logging_ops->log_info("mock_callback", "Buffer received: %.*s, Length: %zu",
                        (int)n, buffer, n);
  mock_callback_counter++;
  return 0;
}
