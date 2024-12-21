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
struct ArrayInputPrivate {
  enum ArraySearchStateEnum state;
  size_t index;
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int array_init(struct Array *array, size_t size) {
  if (!array) {
    return EINVAL;
  }

  array->size = size;
  array->length = 0;

  array->data = malloc(sizeof(void *) * size);
  if (!array->data) {
    return ENOMEM;
  }

  return 0;
};

static void array_destroy(struct Array *array) {
  free(array->data);

  array->data = NULL;
};

static int array_append(struct Array *array, void *element) {
  if (array->length + 1 > array->size) {
    return ENOBUFS;
  }

  array->data[array->length++] = element;

  return 0;
};

static int array_get_element(struct Array array, size_t index,
                             void **value_placeholder) {
  if (index >= array.length) {
    return ENOENT;
  }

  *value_placeholder = array.data[index];

  return 0;
};

static int array_init_search_input(struct ArraySearchInput *search_input,
                                   bool (*filter_func)(void *, void *),
                                   void *filter_data) {

  if (!search_input || !filter_func)
    return EINVAL;

  search_input->filter_func = filter_func;
  search_input->filter_data = filter_data;
  search_input->index = 0;
  search_input->step = 1;
  search_input->state = ARRAY_SEARCH_STATE_NONE;

  return 0;
};

static int
array_init_search_input_with_step(struct ArraySearchInput *search_input,
                                  bool (*filter_func)(void *, void *),
                                  void *filter_data, int step) {
  int err;

  err = array_init_search_input(search_input, filter_func, filter_data);
  if (err) {
    return err;
  }

  search_input->step = step;

  return 0;
};

int array_search_elements(struct Array array, struct ArraySearchInput *input,
                          struct ArraySearchOutput *output) {
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  void *element;
  int err;

  if (!input || !output) {
    return EINVAL;
  }

  // Set defaults for output
  output->result = NULL;
  output->state = ARRAY_SEARCH_STATE_DONE;

  switch (input->state) {
  case ARRAY_SEARCH_STATE_NONE:
    if (input->step >= 0) {
      input->index = 0;
    } else {
      input->index = array.length - 1;
    }
    break;
  case ARRAY_SEARCH_STATE_INPROGRESS:
    input->index += input->step;
    break;
  default:
    return 0;
  }

  for (; input->index < array.length; input->index += input->step) {
    err = array_ops->get_element(array, input->index, &element);
    if (err) {
      return err;
    }

    if (input->filter_func(element, input->filter_data)) {
      input->state = ARRAY_SEARCH_STATE_INPROGRESS;
      output->state = input->state;
      output->result = element;

      return 0;
    }
  }

  return 0;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct ArrayUtilsOps array_utils_ops = {
    .init = array_init,
    .append = array_append,
    .destroy = array_destroy,
    .get_element = array_get_element,
    .search_elements = array_search_elements,
    .init_search_input = array_init_search_input,
    .init_search_input_with_step = array_init_search_input_with_step,
};

struct ArrayUtilsOps *get_array_utils_ops(void) { return &array_utils_ops; };
