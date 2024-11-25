/*******************************************************************************
 * @file display.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library

// App's internal libs
#include "display.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static int display_display(struct DataToDisplay *data);
static void
display_register_module(struct InputRegistrationData *input_registration_data);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct DisplayOps display_ops = {.display = display_display,
                                 .register_module = display_register_module};

struct DisplayOps *get_display_ops(void) { return &display_ops; };
/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int display_display(struct DataToDisplay *data) { return 0; };

static void
display_register_module(struct InputRegistrationData *input_registration_data) {
  return;
};
