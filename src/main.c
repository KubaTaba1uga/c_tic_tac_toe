// TO-DO modularize logging utils
// TO-DO modularize std lib utils
/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils/logging_utils.h"
/*******************************************************************************
 *    DATA STRUCTURES
 ******************************************************************************/
int (*init_operations[1])(void) = {init_loggers};
void (*close_operations[1])(void) = {destroy_loggers};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#ifdef TEST
int main__(void)
#else
int main(void)
#endif
{
  int err;

  // init
  for (size_t i = 0; i < sizeof(init_operations) / sizeof(void *); i++) {
    err = init_operations[i]();

    if (err) {
      return 1;
    }
  }

  log_info("main", "Game initialized");

  // cleanup
  for (size_t i = 0; i < sizeof(close_operations) / sizeof(void *); i++) {
    close_operations[i]();
  }

  return 0;
}
