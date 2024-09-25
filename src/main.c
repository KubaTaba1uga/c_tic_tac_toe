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
#include "input/input.h"
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
  struct InputOps *input_ops;
  struct InitOps *init_ops;
  int err;

  input_ops = get_input_ops();
  init_ops = get_init_ops();

  err = init_ops->initialize_system();
  if (err) {
    return 1;
  }

  logging_utils_ops.log_info("main", "Game initialized");

  // Game logic is triggered by new input. That's why main
  //  game's thread is waiting until all input's are destroyed
  //  by user decision to quit.
  input_ops->wait();

  logging_utils_ops.log_info("main", "Game finished");

  init_ops->destroy_system();

  return 0;
}
