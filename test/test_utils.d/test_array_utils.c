#include <asm-generic/errno.h>
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
  TEST_ASSERT_EQUAL_INT(ENOBUFS, err);
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

static bool compare_ints(void *el, void *target) {
  return *(int *)el == *(int *)target;
}

void test_array_init_search_wrapper_success() {
  struct ArraySearchWrapper *search_wrapper;
  int element = 42;
  int *result = NULL;

  int err = array_ops->init_search_wrapper(&search_wrapper, &element,
                                           compare_ints, (void **)&result);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_NOT_NULL(search_wrapper);

  array_ops->destroy_search_wrapper(&search_wrapper);
}

void test_array_destroy_search_wrapper_success() {
  struct ArraySearchWrapper *search_wrapper;
  int element = 42;
  int *result = NULL;

  array_ops->init_search_wrapper(&search_wrapper, &element, compare_ints,
                                 (void **)&result);

  array_ops->destroy_search_wrapper(&search_wrapper);
  TEST_ASSERT_NULL(search_wrapper);
}

void test_array_get_state_search_wrapper_default_value() {
  struct ArraySearchWrapper *search_wrapper;
  int element = 42;
  int *result = NULL;

  array_ops->init_search_wrapper(&search_wrapper, &element, compare_ints,
                                 (void **)&result);

  enum ArraySearchStateEnum state =
      array_ops->get_state_search_wrapper(search_wrapper);
  TEST_ASSERT_EQUAL_INT(ARRAY_SEARCH_STATE_NONE, state);

  array_ops->destroy_search_wrapper(&search_wrapper);
}

void test_array_search_elements_found() {
  array_search_t search_wrapper;
  int *result = NULL;
  int element = 42;
  int err;

  array_ops->append(test_array, &element);

  err = array_ops->init_search_wrapper(&search_wrapper, &element, compare_ints,
                                       (void **)&result);

  array_ops->search_elements(test_array, search_wrapper);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_PTR(&element, result);

  array_ops->destroy_search_wrapper(&search_wrapper);
}

void test_array_search_elements_not_found() {
  array_search_t search_wrapper;
  int search_target = 24;
  int *result = NULL;
  int element = 42;
  int err;

  array_ops->append(test_array, &element);

  err = array_ops->init_search_wrapper(&search_wrapper, &search_target,
                                       compare_ints, (void **)&result);

  TEST_ASSERT_EQUAL_INT(0, err);

  err = array_ops->search_elements(test_array, search_wrapper);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_NULL(result);

  array_ops->destroy_search_wrapper(&search_wrapper);
}

void test_array_search_multiple_elements() {
  int elements[] = {10, 20, 30, 40, 50};
  size_t num_elements = sizeof(elements) / sizeof(int);
  array_search_t search_wrapper;
  int *result;
  int err;

  // Add elements to the array
  for (size_t i = 0; i < num_elements; i++) {
    array_ops->append(test_array, &elements[i]);
  }

  // Look up each element
  for (size_t i = 0; i < num_elements; i++) {
    err = array_ops->init_search_wrapper(&search_wrapper, &elements[i],
                                         compare_ints, (void **)&result);
    TEST_ASSERT_EQUAL_INT(0, err);

    err = array_ops->search_elements(test_array, search_wrapper);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(elements[i], *result);
    TEST_ASSERT_EQUAL_INT(ARRAY_SEARCH_STATE_INPROGRESS,
                          array_ops->get_state_search_wrapper(search_wrapper));

    array_ops->destroy_search_wrapper(&search_wrapper);
  }
}

void test_array_search_last_element() {
  int elements[] = {10, 20, 30, 40, 50};
  size_t num_elements = sizeof(elements) / sizeof(elements[0]);
  enum ArraySearchStateEnum state;
  array_search_t search_wrapper;
  int *result;
  size_t i;
  int err;

  // Add elements to the array
  for (i = 0; i < num_elements; i++) {
    array_ops->append(test_array, &elements[i]);
  }

  err = array_ops->init_search_wrapper(&search_wrapper,
                                       &elements[num_elements - 1],
                                       compare_ints, (void **)&result);
  TEST_ASSERT_EQUAL_INT(0, err);

  array_ops->search_elements(test_array, search_wrapper);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_NOT_NULL(result);
  TEST_ASSERT_EQUAL_INT(elements[num_elements - 1], *result);
  state = array_ops->get_state_search_wrapper(search_wrapper);
  TEST_ASSERT_EQUAL_INT(ARRAY_SEARCH_STATE_INPROGRESS, state);

  err = array_ops->search_elements(test_array, search_wrapper);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_NULL(result);
  state = array_ops->get_state_search_wrapper(search_wrapper);
  TEST_ASSERT_EQUAL_INT(ARRAY_SEARCH_STATE_DONE, state);

  array_ops->destroy_search_wrapper(&search_wrapper);
}
