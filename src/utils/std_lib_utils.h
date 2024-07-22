#ifndef STD_LIB_UTILS_H
#define STD_LIB_UTILS_H
/*******************************************************************************
 * @file std_lib_utils.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stdbool.h>

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct std_lib_utils_ops {
  unsigned long (*get_now)(void);
  bool (*are_str_eq)(char *str_a, char *str_b);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct std_lib_utils_ops std_lib_utils_ops;

#endif // STD_LIB_UTILS_H
