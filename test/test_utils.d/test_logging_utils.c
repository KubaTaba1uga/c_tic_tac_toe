#include <unity.h>

#include "utils/logging_utils.h"

#include "logging_utils_wrapper.h"

struct test_data {
  int create_log_entry_err;
  int create_log_entry_counter;
};

struct test_data test_data;
static struct LoggingUtilsPrivateOps *logging_priv_ops;
static struct LoggingUtilsOps *logging_ops;

int (*create_log_entry_orig)(char *msg, const char *msg_id,
                             struct stumpless_entry **entry,
                             enum stumpless_severity severity);
static int create_log_entry_mock(char *msg, const char *msg_id,
                                 // Stumpless data
                                 struct stumpless_entry **entry,
                                 enum stumpless_severity severity) {
  test_data.create_log_entry_err =
      create_log_entry_orig(msg, msg_id, entry, severity);

  test_data.create_log_entry_counter++;

  return test_data.create_log_entry_err;
}

void test_create_log_entry_no_variadic_function(void) {
  // Configure test data
  char msg_id[64] = "test_logging_utils";
  char msg[255] = "I like pandas";

  // Configure mocks
  test_data.create_log_entry_err = 0;
  test_data.create_log_entry_counter = 0;

  // Perform test
  logging_ops->log_info(msg_id, msg);

  // Assert test results
  TEST_ASSERT_EQUAL_INT(1, test_data.create_log_entry_counter);

  if (test_data.create_log_entry_err != 0) {
    logging_priv_ops->print_errno();
    TEST_FAIL_MESSAGE("Log entry creation failed.");
  }
}

void test_create_log_entry_variadic_function(void) {
  // Configure test data
  char msg_id[64] = "test_logging_utils";
  char msg[255] = "I like %s";

  // Configure mocks
  test_data.create_log_entry_err = 0;
  test_data.create_log_entry_counter = 0;

  // Perform test
  logging_ops->log_info(msg_id, msg, "pandas");

  // Assert test results
  TEST_ASSERT_EQUAL_INT(1, test_data.create_log_entry_counter);

  if (test_data.create_log_entry_err != 0) {
    logging_priv_ops->print_errno();
    TEST_FAIL_MESSAGE("Log entry creation failed.");
  }
}

void setUp(void) {
  logging_ops = get_logging_utils_ops();
  logging_priv_ops = get_logging_utils_private_ops();

  logging_ops->init();

  create_log_entry_orig = logging_priv_ops->create_log_entry;
  logging_priv_ops->create_log_entry = create_log_entry_mock;
}

void tearDown(void) {
  logging_ops->destroy();
  logging_priv_ops->create_log_entry = create_log_entry_orig;
}
