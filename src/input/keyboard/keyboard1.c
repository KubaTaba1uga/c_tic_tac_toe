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
/* static const char *module_id = INPUT_KEYBOARD1_ID; */

static int keyboard1_module_init(void);
static void keyboard1_module_destroy(void);

static struct init_registration_data init_keyboard1_reg = {
    .id = INPUT_KEYBOARD1_ID,
    .init_func = keyboard1_module_init,
    .destroy_func = keyboard1_module_destroy,
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int keyboard1_module_init(void) { return 0; }

void keyboard1_module_destroy(void) {}

int keyboard1_start(void) { return 0; }

INIT_REGISTER_SUBSYSTEM_CHILD(&init_keyboard1_reg, init_input_reg_p);
