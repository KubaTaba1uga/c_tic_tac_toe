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
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdlib.h>

// App's internal libs
#include "utils/array_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct Array {
  size_t length;
  size_t size;
  void **data;
};

struct ArrayPrivateOps {
  void (*increment_length)(array_t);
};

/*******************************************************************************
 *    API
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

static int array_append(array_t array, void *element) {
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  struct ArrayPrivateOps *priv_ops = array_ops->priv_ops;
  struct Array *tmp_array = array;

  if (array_ops->get_length(tmp_array) + 1 > tmp_array->size) {
    return ENOMEM;
  }

  tmp_array->data[array_ops->get_length(tmp_array)] = element;

  priv_ops->increment_length(tmp_array);

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

static void array_increment_length(array_t array) {
  struct Array *tmp_array = array;

  tmp_array->length++;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct ArrayPrivateOps array_priv_ops = {.increment_length =
                                                    array_increment_length};
static struct ArrayUtilsOps array_utils_ops = {.init = array_init,
                                               .append = array_append,
                                               .get_length = array_get_length,
                                               .get_element = array_get_element,
                                               .priv_ops = &array_priv_ops};

struct ArrayUtilsOps *get_array_utils_ops(void) { return &array_utils_ops; };
