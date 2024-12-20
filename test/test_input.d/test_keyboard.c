#include <pty.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <unity.h>

#include "game/game.h"
#include "init/init.h"
#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_registration.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"
#include "utils/terminal_utils.h"

#include "input_keyboard_wrapper.h"

// Globals for testing
int stdin_backup;
int master_fd; // PTY master file descriptor
int mockup_callback_counter =
    0; // Global variable to count callback invocations
static struct KeyboardOps *keyboard_ops_;
static struct LoggingUtilsOps *logging_ops_;
static struct KeyboardPrivateOps *keyboard_ops_priv;
static struct TerminalUtilsOps *terminal_ops;
keyboard_reg_t test_keyboard_reg;

struct timespec ts = {.tv_sec = 0, .tv_nsec = 50000000};

// Prototypes
static void restore_orig_stdin();
static void setup_pty();
static int mock_keyboard_callback(size_t n, char buffer[n]);

void setUp() {
  struct InitOps *init_ops = get_init_ops();
  logging_ops_ = get_logging_utils_ops();
  keyboard_ops_ = get_keyboard_ops();
  keyboard_ops_priv = get_keyboard_priv_ops();
  terminal_ops = get_terminal_ops();

  // Disable game init and destroy
  init_game_reg.init = NULL;
  init_game_reg.destroy = NULL;
  init_ops->initialize_system();

  setup_pty();

  // Initialize keyboard registration for tests
  struct KeyboardRegistrationOps *keyboard_reg_ops =
      get_keyboard_registration_ops();
  TEST_ASSERT_EQUAL_INT(0, keyboard_reg_ops->init(&test_keyboard_reg,
                                                  "test_module",
                                                  mock_keyboard_callback));

  mockup_callback_counter = 0; // Reset callback counter
}

void tearDown() {
  struct InitOps *init_ops = get_init_ops();

  restore_orig_stdin();

  // Destroy the test registration
  struct KeyboardRegistrationOps *keyboard_reg_ops =
      get_keyboard_registration_ops();
  keyboard_reg_ops->destroy(&test_keyboard_reg);

  init_ops->destroy_system();
}

void test_process_single_stdin() {
  // Start the keyboard subsystem
  keyboard_ops_->register_callback(test_keyboard_reg);
  keyboard_ops_->start();

  // Write one character to the PTY master
  write(master_fd, "x", strlen("x")); // Simulate typing 'x'

  thrd_sleep(&ts, NULL); // Allow thread to process input

  keyboard_ops_->stop();

  // Check if the callback was executed exactly once
  TEST_ASSERT_EQUAL_INT(1, mockup_callback_counter);
}

void test_process_double_stdin() {
  // Start the keyboard subsystem
  keyboard_ops_->register_callback(test_keyboard_reg);
  keyboard_ops_->start();

  // Write one character to the PTY master
  write(master_fd, "x", strlen("x")); // Simulate typing 'x'

  thrd_sleep(&ts, NULL); // Allow thread to process input

  write(master_fd, "x", strlen("x")); // Simulate typing 'x'

  thrd_sleep(&ts, NULL); // Allow thread to process input

  keyboard_ops_->stop();

  // Check if the callback was executed exactly once
  TEST_ASSERT_EQUAL_INT(2, mockup_callback_counter);
}

void test_process_multiple_inputs() {
  // Array of inputs to simulate
  const char *inputs[] = {"abc", "bca", "cab", "def", "efd"};
  size_t input_count = sizeof(inputs) / sizeof(const char *);

  // Start the keyboard subsystem
  keyboard_ops_->register_callback(test_keyboard_reg);
  keyboard_ops_->start();

  // Write each input to the PTY master
  for (size_t i = 0; i < input_count; i++) {
    write(master_fd, inputs[i], strlen(inputs[i])); // Simulate typing input
    thrd_sleep(&ts, NULL); // Allow thread to process input
    TEST_ASSERT_EQUAL_INT(i + 1, mockup_callback_counter);
  }

  keyboard_ops_->stop();
}

void setup_pty() {
  int slave_fd;

  // Backup the current stdin
  stdin_backup = dup(STDIN_FILENO);

  // Open a pseudo-terminal
  if (openpty(&master_fd, &slave_fd, NULL, NULL, NULL) == -1) {
    perror("openpty");
    exit(EXIT_FAILURE);
  }

  // Redirect stdin to the PTY slave
  dup2(slave_fd, STDIN_FILENO);
  close(slave_fd); // Close the slave_fd in the test process

  terminal_ops->disable_canonical_mode(master_fd);
}

void restore_orig_stdin() {
  // Restore the original stdin
  dup2(stdin_backup, STDIN_FILENO);
  close(stdin_backup);
  close(master_fd);
}

int mock_keyboard_callback(size_t n, char buffer[n]) {
  // Log the callback invocation
  logging_ops_->log_info("mock_keyboard_callback", "Buffer: %.*s, Size: %zu",
                         (int)n, buffer, n);

  // Increment the callback counter
  mockup_callback_counter++;

  // Ensure callback receives the correct single character
  /* TEST_ASSERT_EQUAL_INT(1, n);            // Only one character */
  /* TEST_ASSERT_EQUAL_CHAR('x', buffer[0]); // The character should be 'x' */

  return 0;
}
