#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include <unity.h>

#include "utils/array_utils.h"

#include "array_utils_wrapper.h"

static struct ArrayPrivateOps *array_ops_;
static struct ArrayUtilsOps *array_ops;

static size_t test_array_size = 10;
static array_t test_array;

void setUp() {
  array_ops = get_array_utils_ops();
  array_ops_ = get_array_priv_ops();

  int err = array_ops->init(&test_array, test_array_size);
  TEST_ASSERT_EQUAL_INT(0, err);
}

void tearDown() { array_ops->destroy(&test_array); }

void test_array_init_success() {
  TEST_ASSERT_EQUAL_INT(0, array_ops->get_length(test_array));
  TEST_ASSERT_EQUAL_INT(test_array_size, array_ops_->get_size(test_array));
}

void test_array_append_success() {
  int element = 42;

  int err = array_ops->append(test_array, &element);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(1, array_ops->get_length(test_array));
  TEST_ASSERT_EQUAL_PTR(&element, array_ops->get_element(test_array, 0));
}

void test_array_append_failure_no_space() {
  int element = 42;

  // Fill the array to capacity
  for (size_t i = 0; i < 10; i++) {
    int err = array_ops->append(test_array, &element);
    TEST_ASSERT_EQUAL_INT(0, err);
  }

  // Try adding another element
  int err = array_ops->append(test_array, &element);
  TEST_ASSERT_EQUAL_INT(ENOMEM, err);
}

void test_array_get_length() {
  TEST_ASSERT_EQUAL_INT(0, array_ops->get_length(test_array));

  int element = 42;
  array_ops->append(test_array, &element);

  TEST_ASSERT_EQUAL_INT(1, array_ops->get_length(test_array));
}

void test_array_get_element_valid_index() {
  int element = 42;
  array_ops->append(test_array, &element);

  int *retrieved_element = array_ops->get_element(test_array, 0);
  TEST_ASSERT_NOT_NULL(retrieved_element);
  TEST_ASSERT_EQUAL_INT(42, *retrieved_element);
}

void test_array_get_element_invalid_index() {
  void *retrieved_element = array_ops->get_element(test_array, 1);

  TEST_ASSERT_NULL(retrieved_element);
}

void test_array_increment_length_private() {
  array_ops_->increment_length(test_array);

  TEST_ASSERT_EQUAL_INT(1, array_ops->get_length(test_array));
}
