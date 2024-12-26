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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// App's internal libs
#include "config/config.h"
#include "display/display.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static const char main_id[] = "main";

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

/*******************************************************************************
 *    API
 ******************************************************************************/
#ifdef TEST
int main__(void)
#else
int main(void)
#endif
{
  struct LoggingUtilsOps *logging_ops;
  struct InputOps *input_ops;
  struct InitOps *init_ops;
  int err;

  logging_ops = get_logging_utils_ops();
  input_ops = get_input_ops();
  init_ops = get_init_ops();

  err = init_ops->initialize();
  if (err) {
    logging_ops->log_err(main_id, "Unable to initialize game: %s.",
                         strerror(err));
    return 1;
  }

  // Game logic is triggered by new input. That's why main
  //  game's thread is waiting until all input's are destroyed
  //  by user's decision to quit.
  input_ops->wait();

  logging_ops->log_info(main_id, "Game finished");

  init_ops->destroy();

  return 0;
}
