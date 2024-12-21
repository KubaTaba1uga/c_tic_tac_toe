#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include <unity.h>

#include "utils/array_utils.h"

static struct ArrayUtilsOps *array_ops;
static struct Array test_array;
static const size_t test_array_size = 10;

void setUp() {
  array_ops = get_array_utils_ops();
  int err = array_ops->init(&test_array, test_array_size);
  TEST_ASSERT_EQUAL_INT(0, err);
}

void tearDown() { array_ops->destroy(&test_array); }

void test_array_init_success() {
  TEST_ASSERT_EQUAL_INT(0, test_array.length);
  TEST_ASSERT_EQUAL_INT(test_array_size, test_array.size);
}

void test_array_append_success() {
  int element = 42;
  int err = array_ops->append(&test_array, &element);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(1, test_array.length);

  void *retrieved_element;
  err = array_ops->get_element(test_array, 0, &retrieved_element);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_PTR(&element, retrieved_element);
}

void test_array_append_failure_no_space() {
  int element = 42;

  // Fill the array to capacity
  for (size_t i = 0; i < test_array_size; i++) {
    int err = array_ops->append(&test_array, &element);
    TEST_ASSERT_EQUAL_INT(0, err);
  }

  // Try adding another element
  int err = array_ops->append(&test_array, &element);
  TEST_ASSERT_EQUAL_INT(ENOBUFS, err);
}

void test_array_get_element_valid_index() {
  int element = 42;
  array_ops->append(&test_array, &element);

  void *retrieved_element;
  int err = array_ops->get_element(test_array, 0, &retrieved_element);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_PTR(&element, retrieved_element);
}

void test_array_get_element_invalid_index() {
  void *retrieved_element;
  int err = array_ops->get_element(test_array, 1, &retrieved_element);
  TEST_ASSERT_EQUAL_INT(ENOENT, err);
}

static bool filter_even_numbers(void *el, void *unused) {
  return (*(int *)el % 2) == 0;
}

void test_array_search_elements_found() {
  int elements[] = {1, 2, 3, 4, 5};
  for (size_t i = 0; i < 5; i++) {
    array_ops->append(&test_array, &elements[i]);
  }

  struct ArraySearchInput input;
  struct ArraySearchOutput output;

  array_ops->init_search_input(&input, filter_even_numbers, NULL);
  int err = array_ops->search_elements(test_array, &input, &output);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_NOT_NULL(output.result);
  TEST_ASSERT_EQUAL_INT(2, *(int *)output.result);
}

void test_array_search_elements_not_found() {
  int elements[] = {1, 3, 5, 7, 9};
  for (size_t i = 0; i < 5; i++) {
    array_ops->append(&test_array, &elements[i]);
  }

  struct ArraySearchInput input;
  struct ArraySearchOutput output;

  array_ops->init_search_input(&input, filter_even_numbers, NULL);
  int err = array_ops->search_elements(test_array, &input, &output);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_NULL(output.result);
}
