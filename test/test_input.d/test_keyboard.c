#include <pty.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <unity.h>

#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_keys_mapping.h"
#include "input/keyboard/keyboard_keys_mapping_1.h"
#include "utils/logging_utils.h"
#include "utils/terminal_utils.h"

#include "input_keyboard1_wrapper.h"
#include "input_keyboard_wrapper.h"

// Global Variables
static int stdin_backup;
static int master_fd; // PTY master file descriptor
static int callback_counter = 0;

static struct InputOps *input_ops;
static struct KeyboardOps *keyboard_ops;
static struct KeyboardOps *keyboard_ops;
static struct LoggingUtilsOps *logging_ops;
static struct TerminalUtilsOps *terminal_ops;
static struct KeyboardKeysMappingOps *mappings_ops;
static struct KeyboardKeysMapping1Ops *mapping_1_ops;
static struct KeyboardKeysMapping1PrivOps *mapping_1_priv_ops;

struct timespec ts = {.tv_sec = 0, .tv_nsec = 50000000};

// Helper Function Prototypes
static void setup_pty(void);
static void restore_original_stdin(void);
static int
mock_keyboard_callback(const struct KeyboardKeysMappingCallbackInput *input,
                       struct KeyboardKeysMappingCallbackOutput *output);
static int mock_input_callback(enum InputEvents, input_device_id_t);

// Test Setup
void setUp(void) {
  int err;

  input_ops = get_input_ops();
  keyboard_ops = get_keyboard_ops();
  logging_ops = get_logging_utils_ops();
  terminal_ops = get_terminal_ops();
  mapping_1_priv_ops = get_keyboard_keys_mapping_1_priv_ops();
  mapping_1_ops = get_keyboard_keys_mapping_1_ops();
  mappings_ops = get_keyboard_keys_mapping_ops();

  mapping_1_priv_ops->keyboard_callback = mock_keyboard_callback;

  err = logging_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = get_input_ops()->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = keyboard_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = mapping_1_ops->init();
  TEST_ASSERT_EQUAL_INT(0, err);

  err = input_ops->set_callback(
      (struct InputSetCallbackInput){.callback = mock_input_callback,
                                     .device_id = 0},
      &(struct InputSetCallbackOutput){});
  TEST_ASSERT_EQUAL_INT(0, err);

  setup_pty();

  callback_counter = 0;
}

// Test Teardown
void tearDown(void) {
  restore_original_stdin();
  keyboard_ops->destroy();
  logging_ops->destroy();
}

// Test Single Input
void test_keyboard_single_input(void) {
  int err;

  err = keyboard_ops->start();
  TEST_ASSERT_EQUAL_INT(0, err);

  write(master_fd, "w", 1);

  thrd_sleep(&ts, NULL); // Allow processing

  err = keyboard_ops->stop();
  TEST_ASSERT_EQUAL_INT(0, err);

  TEST_ASSERT_EQUAL_INT(1, callback_counter);
}

// Test Multiple Inputs
void test_keyboard_multiple_inputs(void) {
  int err;

  err = keyboard_ops->start();
  TEST_ASSERT_EQUAL_INT(0, err);

  const char *inputs[] = {"wsad", "qe", "ijkl"};
  for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    write(master_fd, inputs[i], strlen(inputs[i]));
    thrd_sleep(&ts, NULL); // Allow processing
    TEST_ASSERT_EQUAL_INT((int)(i + 1), callback_counter);
  }

  err = keyboard_ops->stop();
  TEST_ASSERT_EQUAL_INT(0, err);
}

// Test Thread Restart
void test_keyboard_thread_restart(void) {
  int err;

  err = keyboard_ops->start();
  TEST_ASSERT_EQUAL_INT(0, err);

  write(master_fd, "q", 1);
  thrd_sleep(&ts, NULL);
  keyboard_ops->stop();
  TEST_ASSERT_EQUAL_INT(1, callback_counter);

  err = keyboard_ops->start();
  TEST_ASSERT_EQUAL_INT(0, err);

  write(master_fd, "e", 1);
  thrd_sleep(&ts, NULL);

  err = keyboard_ops->stop();
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(2, callback_counter);
}

// PTY Setup
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

// Restore Original Stdin
void restore_original_stdin(void) {
  dup2(stdin_backup, STDIN_FILENO);
  close(stdin_backup);
  close(master_fd);
}

// Mock Callback
int mock_keyboard_callback(const struct KeyboardKeysMappingCallbackInput *input,
                           struct KeyboardKeysMappingCallbackOutput *output) {
  logging_ops->log_info("mock_keyboard_callback", "Buffer: %.*s, Length: %zu",
                        (int)input->n, input->buffer, input->n);
  callback_counter++;
  output->input_event = INPUT_EVENT_UP; // Mocked event
  return 0;
}

int mock_input_callback(enum InputEvents input_event,
                        input_device_id_t device_id) {
  return 0;
};
