/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// App's internal libs
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS
 ******************************************************************************/
static const char *module_id = "keyboard1";

static int keyboard1_module_init(void);
static int keyboard1_process(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int keyboard1_module_init(void) {
  struct input_registration_data input_registration_data;

  memset(&input_registration_data, 0, sizeof(input_registration_data));
  input_registration_data.id = module_id;
  input_registration_data.process = keyboard1_process;

  return 0;
}

int keyboard1_process(void) { return 0; }
