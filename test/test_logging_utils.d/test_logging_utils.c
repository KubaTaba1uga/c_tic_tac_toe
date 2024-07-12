// Logging library
#include <unity.h>

#include "utils/logging_utils.c"

int create_log_entry_err;
int (*create_log_entry_orig)(char *msg, char *msg_id,
                             struct stumpless_entry **entry,
                             enum stumpless_severity severity);
static int create_log_entry_mock(char *msg, char *msg_id,
                                 // Stumpless data
                                 struct stumpless_entry **entry,
                                 enum stumpless_severity severity) {
  create_log_entry_err = create_log_entry_orig(msg, msg_id, entry, severity);

  return create_log_entry_err;
}

void test_create_log_entry_no_variadic_function(void) {
  // Configure test data
  char msg_id[64] = "test_logging_utils";
  char msg[255] = "I like pandas";
  create_log_entry_err = 0;

  // Configure mocks
  create_log_entry_orig = logging_utils_priv_ops.create_log_entry;
  logging_utils_priv_ops.create_log_entry = create_log_entry_mock;

  // Perform test
  logging_utils_ops.log_info(msg_id, msg);

  // Assert test result
  if (create_log_entry_err != 0) {
    logging_utils_priv_ops.print_errno();
    TEST_FAIL_MESSAGE("Log entry creation failed.");
  }
}
