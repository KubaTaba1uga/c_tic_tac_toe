#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unity.h>

// Include the header files
#include "init/init.h"
#include "utils/array_utils.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"
#include "utils/subsystem_utils.h"

#include "subsystem_utils_wrapper.h"

// Mock data
#define MAX_REGISTRATIONS 10
#define TEST_SUBSYSTEM_ID "test_subsystem"
#define TEST_MODULE_ID "test_module"
#define TEST_MODULE_DATA "module_data"

// Declare subsystem and dependencies
static struct SubsystemUtilsOps *subsystem_utils_ops;
static struct LoggingUtilsOps *logging_ops;
static struct ArrayUtilsOps *array_ops;

static subsystem_t test_subsystem;

// Mock filter function
bool mock_filter_func(const char *id, void *private_data, void *filter_data) {
  return strcmp(id, (char *)filter_data) == 0;
}

// Test setup
void setUp() {
  int err;

  subsystem_utils_ops = get_subsystem_utils_ops();
  logging_ops = get_logging_utils_ops();
  array_ops = get_array_utils_ops();

  err = init_subsystem_utils_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  subsystem_utils_ops->init(&test_subsystem, TEST_SUBSYSTEM_ID,
                            MAX_REGISTRATIONS);
}

// Test teardown
void tearDown() { subsystem_utils_ops->destroy(&test_subsystem); }

// Test subsystem initialization
void test_subsystem_utils_init_success() {
  subsystem_t subsystem;
  int result = subsystem_utils_ops->init(&subsystem, TEST_SUBSYSTEM_ID,
                                         MAX_REGISTRATIONS);
  TEST_ASSERT_EQUAL_INT(0, result);
  TEST_ASSERT_NOT_NULL(subsystem);
  TEST_ASSERT_EQUAL_STRING(TEST_SUBSYSTEM_ID, subsystem->id);
  subsystem_utils_ops->destroy(&subsystem);
}

// Test subsystem destruction
void test_subsystem_utils_destroy_success() {
  subsystem_t subsystem;
  subsystem_utils_ops->init(&subsystem, TEST_SUBSYSTEM_ID, MAX_REGISTRATIONS);
  subsystem_utils_ops->destroy(&subsystem);
  TEST_ASSERT_NULL(subsystem);
}

// Test module registration
void test_subsystem_utils_register_module_success() {
  int result = subsystem_utils_ops->register_module(
      test_subsystem, TEST_MODULE_ID, TEST_MODULE_DATA);
  TEST_ASSERT_EQUAL_INT(0, result);
  TEST_ASSERT_EQUAL_INT(1,
                        array_ops->get_length(test_subsystem->registrations));
}

// Test searching modules
void test_subsystem_utils_search_modules_found() {
  subsystem_utils_ops->register_module(test_subsystem, TEST_MODULE_ID,
                                       TEST_MODULE_DATA);

  module_search_t search_wrapper;
  void *result = NULL;
  subsystem_utils_ops->init_search_module_wrapper(
      &search_wrapper, (void *)TEST_MODULE_ID, mock_filter_func, &result);

  int search_result =
      subsystem_utils_ops->search_modules(test_subsystem, search_wrapper);
  TEST_ASSERT_EQUAL_INT(0, search_result);
  TEST_ASSERT_NOT_NULL(result);
  subsystem_utils_ops->destroy_search_module_wrapper(&search_wrapper);
}

// Test searching modules not found
void test_subsystem_utils_search_modules_not_found() {
  subsystem_utils_ops->register_module(test_subsystem, TEST_MODULE_ID,
                                       TEST_MODULE_DATA);

  module_search_t search_wrapper;
  void *result = NULL;
  subsystem_utils_ops->init_search_module_wrapper(
      &search_wrapper, (void *)"non_existent", mock_filter_func, &result);

  int search_result =
      subsystem_utils_ops->search_modules(test_subsystem, search_wrapper);
  TEST_ASSERT_EQUAL_INT(0, search_result);
  TEST_ASSERT_NULL(result);
  subsystem_utils_ops->destroy_search_module_wrapper(&search_wrapper);
}

// Test get search wrapper state
void test_subsystem_utils_get_search_wrapper_state() {
  module_search_t search_wrapper;
  void *result = NULL;
  subsystem_utils_ops->init_search_module_wrapper(
      &search_wrapper, (void *)TEST_MODULE_ID, mock_filter_func, &result);

  enum ArraySearchStateEnum state =
      subsystem_utils_ops->get_state_search_module_wrapper(search_wrapper);
  TEST_ASSERT_EQUAL_INT(ARRAY_SEARCH_STATE_NONE, state);

  subsystem_utils_ops->destroy_search_module_wrapper(&search_wrapper);
}

// Main function to run all tests
int main() {
  UNITY_BEGIN();

  RUN_TEST(test_subsystem_utils_init_success);
  RUN_TEST(test_subsystem_utils_destroy_success);
  RUN_TEST(test_subsystem_utils_register_module_success);
  RUN_TEST(test_subsystem_utils_search_modules_found);
  RUN_TEST(test_subsystem_utils_search_modules_not_found);
  RUN_TEST(test_subsystem_utils_get_search_wrapper_state);

  return UNITY_END();
}
