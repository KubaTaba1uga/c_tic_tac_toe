// TO-DO modularize std lib utils
/*******************************************************************************
 * @file main.c
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// App's internal libs
#include "init/init.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
#ifdef TEST
int main__(void)
#else
int main(void)
#endif
{
  int err;

  err = init_ops.initialize_system();
  if (err) {
    return 1;
  }

  logging_utils_ops.log_info("main", "Game initialized");

  init_ops.destroy_system();

  return 0;
}
