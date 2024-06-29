// TO-DO modularize std lib utils
/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "init/init.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    DATA STRUCTURES
 ******************************************************************************/
/* int (*init_operations[1])(void) = {}; */
/* void (*close_operations[1])(void) = { */

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#ifdef TEST
int main__(void)
#else
int main(void)
#endif
{
  /* int err; */

  /* // init */
  /* for (size_t i = 0; i < sizeof(init_operations) / sizeof(void *); i++) { */
  /*   err = init_operations[i](); */

  /*   if (err) { */
  /*     return 1; */
  /*   } */
  /* } */

  initialize_system();

  logging_utils_ops.log_info("main", "Game initialized");

  logging_utils_ops.destroy_loggers();

  /* // cleanup */
  /* for (size_t i = 0; i < sizeof(close_operations) / sizeof(void *); i++) { */
  /*   close_operations[i](); */
  /* } */

  return 0;
}
