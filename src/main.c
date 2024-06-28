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

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#ifdef TEST
int main__(void)
#else
int main(void)
#endif
{
  // init logging
  init_loggers();

  log_info("main", "Game started");

  // cleanup logging
  destroy_loggers();

  return 0;
}
