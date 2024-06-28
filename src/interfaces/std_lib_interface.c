/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <stdlib.h>

// App
#include "std_lib_interface.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
void *app_malloc(size_t size) { return malloc(size); }

void *app_realloc(void *p, size_t size) { return realloc(p, size); }

void app_free(void *p) { free(p); }

int app_exit(int exit_code) { exit(exit_code); }
