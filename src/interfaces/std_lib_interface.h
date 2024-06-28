#ifndef _std_lib_interface_h
#define _std_lib_interface_h

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C Standard library
#include <stddef.h>

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
void *app_malloc(size_t size);
void *app_realloc(void *p, size_t size);
void app_free(void *p);
int app_exit(int exit_code);

#endif
