/*******************************************************************************
 * @file std_lib_utils.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <stdbool.h>
#include <string.h>
#include <time.h>

// App's internal libs
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static unsigned long get_current_time(void);
static bool are_strings_equal(char *str_a, char *str_b);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct StdLibUtilsOps std_lib_utils_ops = {.get_now = get_current_time,
                                           .are_str_eq = are_strings_equal};

struct StdLibUtilsOps *get_std_lib_utils_ops(void) {
  return &std_lib_utils_ops;
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/

/*******************************************************************************
 *    API
 ******************************************************************************/
unsigned long get_current_time(void) {
  // Return number of seconds since Epoch.
  // On error returns 0.
  time_t now = time(NULL);
  if (now == (time_t)(-1))
    return 0;

  return (unsigned long)now;
}

bool are_strings_equal(char *str_a, char *str_b) {
  // Return True if A and B are equal, False otherwise.
  return strcmp(str_a, str_b) == 0;
}
