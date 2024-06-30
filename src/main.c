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

  err = initialize_system();
  if (err) {
    return 1;
  }

  logging_utils_ops.log_info("main", "Game initialized");

  destroy_system();

  return 0;
}
