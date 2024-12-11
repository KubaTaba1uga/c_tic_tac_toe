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
typedef struct Array *array_t;
typedef struct ArraySearchWrapper *array_search_t;

enum ArraySearchStateEnum {
  ARRAY_SEARCH_STATE_NONE = 0,
  ARRAY_SEARCH_STATE_INPROGRESS,
  ARRAY_SEARCH_STATE_DONE,
  ARRAY_SEARCH_STATE_INVALID,
};

struct ArrayUtilsOps {
  int (*init)(array_t *, size_t);
  void (*destroy)(array_t *);
  int (*append)(array_t, void *);
  size_t (*get_length)(array_t);
  void *(*get_element)(array_t, size_t);
  int (*search_elements)(array_t, array_search_t);
  int (*init_search_wrapper)(array_search_t *, void *, bool (*)(void *, void *),
                             void **);
  void (*destroy_search_wrapper)(array_search_t *);
  enum ArraySearchStateEnum (*get_state_search_wrapper)(array_search_t);
  void (*set_step_search_wrapper)(array_search_t search_wrap, int step);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct ArrayUtilsOps *get_array_utils_ops(void);

#endif // ARRAY_UTILS_H
