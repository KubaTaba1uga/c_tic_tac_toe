#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H
/*******************************************************************************
 * @file array_utils.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stdbool.h>
#include <stddef.h>

/*******************************************************************************
 *    API
 ******************************************************************************/
struct Array {
  size_t length;
  size_t size;
  void **data;
};

enum ArraySearchStateEnum {
  ARRAY_SEARCH_STATE_NONE = 0,
  ARRAY_SEARCH_STATE_INPROGRESS,
  ARRAY_SEARCH_STATE_DONE,
  ARRAY_SEARCH_STATE_INVALID,
};

struct ArraySearchInput {
  bool (*filter_func)(void *, void *);
  void *filter_data;
  int step;
  enum ArraySearchStateEnum state;
  size_t index;
};

struct ArraySearchOutput {
  enum ArraySearchStateEnum state;
  void *result;
};

struct ArrayUtilsOps {
  int (*init)(struct Array *, size_t);
  void (*destroy)(struct Array *);
  int (*append)(struct Array *, void *);
  int (*get_element)(struct Array, size_t, void **);
  int (*init_search_input)(struct ArraySearchInput *, bool (*)(void *, void *),
                           void *);
  int (*init_search_input_with_step)(struct ArraySearchInput *,
                                     bool (*)(void *, void *), void *, int);
  int (*search_elements)(struct Array, struct ArraySearchInput *,
                         struct ArraySearchOutput *);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct ArrayUtilsOps *get_array_utils_ops(void);

#endif // ARRAY_UTILS_H
