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
#include <errno.h>
#include <stddef.h>

// App's internal libs
#include "config/config.h"
#include "display/display.h"
#include "init/init.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_DISPLAY_REGISTRATIONS 100

struct DisplaySubsystem {
  struct DisplayRegistrationData *registrations[MAX_DISPLAY_REGISTRATIONS];
  size_t count;
};

static char module_id[] = "display_subsystem";
static struct DisplaySubsystem display_subsystem;
static int display_init(void);
static int display_display(struct DataToDisplay *data);
static struct DisplaySubsystem *display_get_subsystem(void);
static void
display_register_module(struct DisplayRegistrationData *registration_data);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct DisplayPrivateOps {
  struct DisplaySubsystem *(*get_subsystem)(void);
};

static struct DisplayPrivateOps display_private_ops = {
    .get_subsystem = display_get_subsystem};

static struct DisplayOps display_ops = {.display = display_display,
                                        .register_module =
                                            display_register_module,
                                        .private_ops = &display_private_ops};

struct DisplayOps *get_display_ops(void) { return &display_ops; };

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_display_reg = {
    .id = module_id,
    .init = display_init,
    .destroy = NULL,
};

/*******************************************************************************
 *    API
 ******************************************************************************/
static int display_init(void) {
  struct ConfigRegistrationData display_env;
  struct LoggingUtilsOps *logging_ops;
  struct ConfigOps *config_ops;
  int err;

  logging_ops = get_logging_utils_ops();
  config_ops = get_config_ops();

  display_env.var_name = "display";
  display_env.default_value = DISPLAY_CLI_NAME;

  err = config_ops->register_var(display_env);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to register display configuration var.");
    return err;
  }

  return 0;
};

static int display_display(struct DataToDisplay *data) {
  struct LoggingUtilsOps *logging_ops;
  struct DisplaySubsystem *subsystem;
  struct StdLibUtilsOps *std_lib_ops;
  struct ConfigOps *config_ops;
  char *display_env;

  subsystem = display_private_ops.get_subsystem();
  logging_ops = get_logging_utils_ops();
  std_lib_ops = get_std_lib_utils_ops();
  config_ops = get_config_ops();

  display_env = config_ops->get_var("display");

  for (size_t i = 0; i < subsystem->count; i++) {

    if (std_lib_ops->are_str_eq((char *)subsystem->registrations[i]->id,
                                display_env)) {
      return subsystem->registrations[i]->display(data);
    }
  }

  logging_ops->log_err(module_id, "Unable to find matching display for %s.",
                       display_env);

  return EINVAL;
};

static void
display_register_module(struct DisplayRegistrationData *registration_data) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  struct DisplaySubsystem *subsystem = display_private_ops.get_subsystem();

  if (subsystem->count < MAX_DISPLAY_REGISTRATIONS) {
    subsystem->registrations[subsystem->count++] = registration_data;
  } else {
    logging_ops->log_err(module_id,
                         "Unable to register %s in display, "
                         "no enough space in `registrations` array.",
                         registration_data->id);
  }
};

struct DisplaySubsystem *display_get_subsystem(void) {
  return &display_subsystem;
};

INIT_REGISTER_SUBSYSTEM(init_display_reg_p, INIT_MODULE_ORDER_DISPLAY);
