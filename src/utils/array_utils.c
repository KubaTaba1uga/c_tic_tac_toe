/*******************************************************************************
 * @file array_utils.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// App's internal libs
#include "array_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct Array {
  size_t length;
  size_t size;
  void **data;
};

struct ArraySearchState {
  enum ArraySearchStateEnum state;
  size_t index;
};

struct ArraySearchWrapper {
  void *filter_data;
  bool (*filter_func)(void *, void *);
  void **result_placeholder;
  struct ArraySearchState state;
};

struct ArrayPrivateOps {
  void (*increment_length)(array_t);
  size_t (*get_size)(array_t);
};

struct ArrayPrivateOps *get_array_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int array_init(array_t *array, size_t size) {
  struct Array *new_array;

  new_array = malloc(sizeof(struct Array));
  if (!new_array) {
    return ENOMEM;
  }

  new_array->size = size;
  new_array->length = 0;

  new_array->data = malloc(sizeof(void *) * size);
  if (!new_array->data) {
    return ENOMEM;
  }

  *array = new_array;

  return 0;
};

static void array_destroy(array_t *array) {
  struct Array *tmp_array = *array;
  free(tmp_array->data);
  free(tmp_array);

  *array = NULL;
};

static int array_append(array_t array, void *element) {
  struct ArrayPrivateOps *array_ops_ = get_array_priv_ops();
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  struct Array *tmp_array = array;

  if (array_ops->get_length(tmp_array) + 1 > array_ops_->get_size(tmp_array)) {
    return ENOBUFS;
  }

  tmp_array->data[array_ops->get_length(tmp_array)] = element;

  array_ops_->increment_length(tmp_array);

  return 0;
};

static size_t array_get_length(array_t array) {
  struct Array *tmp_array = array;

  return tmp_array->length;
};

static void *array_get_element(array_t array, size_t index) {
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  struct Array *tmp_array = array;

  if (index >= array_ops->get_length(tmp_array)) {
    return NULL;
  }

  return tmp_array->data[index];
};

static int array_init_search_wrapper(array_search_t *search_wrap,
                                     void *filter_data,
                                     bool (*filter_func)(void *, void *),
                                     void **result_placeholder) {
  array_search_t tmp_wrap;

  if (!search_wrap || !filter_data || !filter_func || !result_placeholder)
    return EINVAL;

  tmp_wrap = malloc(sizeof(struct ArraySearchWrapper));
  if (!tmp_wrap) {
    return ENOMEM;
  }

  tmp_wrap->state.index = 0;
  *result_placeholder = NULL;
  tmp_wrap->filter_data = filter_data;
  tmp_wrap->filter_func = filter_func;
  tmp_wrap->state.state = ARRAY_SEARCH_STATE_NONE;
  tmp_wrap->result_placeholder = result_placeholder;

  *search_wrap = tmp_wrap;

  return 0;
};

void array_destroy_search_wrapper(array_search_t *search_wrap) {
  array_search_t tmp_wrap;

  if (!search_wrap)
    return;

  tmp_wrap = *search_wrap;

  free(tmp_wrap);

  *search_wrap = NULL;
};

int array_search_elements(array_t array, array_search_t search_wrap) {
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  void *element;
  size_t i;

  if (!array_ops) {
    return ENODATA;
  }

  if (!array || !search_wrap || !search_wrap->filter_data ||
      !search_wrap->filter_func || !search_wrap->result_placeholder) {
    return EINVAL;
  }

  switch (search_wrap->state.state) {
  case ARRAY_SEARCH_STATE_NONE:
    i = 0;
    break;
  case ARRAY_SEARCH_STATE_INPROGRESS:
    i = search_wrap->state.index + 1;
    break;
  default:
    return 0;
  }

  // Handle no element
  *search_wrap->result_placeholder = NULL;
  search_wrap->state.state = ARRAY_SEARCH_STATE_DONE;

  for (; i <= array_ops->get_length(array); i++) {
    element = array_ops->get_element(array, i);
    if (!element) {
      return 0;
    }

    if (search_wrap->filter_func(element, search_wrap->filter_data)) {
      search_wrap->state.index = i;
      *search_wrap->result_placeholder = element;
      search_wrap->state.state = ARRAY_SEARCH_STATE_INPROGRESS;
      return 0;
    }
  }

  return 0;
};

enum ArraySearchStateEnum array_search_get_state(array_search_t search_wrap) {
  return search_wrap->state.state;
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static void array_increment_length(array_t array) {
  struct Array *tmp_array = array;

  tmp_array->length++;
};

static size_t array_get_size(array_t array) {
  struct Array *tmp_array = array;

  return tmp_array->size;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct ArrayPrivateOps array_priv_ops = {
    .increment_length = array_increment_length, .get_size = array_get_size};

static struct ArrayUtilsOps array_utils_ops = {
    .init = array_init,
    .append = array_append,
    .destroy = array_destroy,
    .get_length = array_get_length,
    .get_element = array_get_element,
    .search_elements = array_search_elements,
    .init_search_wrapper = array_init_search_wrapper,
    .destroy_search_wrapper = array_destroy_search_wrapper,
    .get_state_search_wrapper = array_search_get_state};

struct ArrayPrivateOps *get_array_priv_ops(void) { return &array_priv_ops; };
;

struct ArrayUtilsOps *get_array_utils_ops(void) { return &array_utils_ops; };
