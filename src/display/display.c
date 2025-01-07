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
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "static_array_lib.h"

// App's internal libs
#include "config/config.h"
#include "display/display.h"
#include "init/init.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_DISPLAY_REGISTRATIONS 10

typedef struct DisplaySubsystem DisplaySubsystem;

struct DisplaySubsystem {
  SARRS_FIELD(displays, struct DisplayDisplay, MAX_DISPLAY_REGISTRATIONS);
};

SARRS_DECL(DisplaySubsystem, displays, struct DisplayDisplay,
           MAX_DISPLAY_REGISTRATIONS);

static char module_id[] = "display_subsystem";
static struct DisplaySubsystem display_subsystem;
static struct LoggingUtilsOps *logging_ops;
static struct ConfigOps *config_ops;
static struct StdLibUtilsOps *std_lib_ops;

/*******************************************************************************
 *    API
 ******************************************************************************/
static int display_init(void) {
  logging_ops = get_logging_utils_ops();
  config_ops = get_config_ops();
  std_lib_ops = get_std_lib_utils_ops();
  return 0;
};

static int display_display(struct DisplayData *data) {
  struct DisplayDisplay *display;
  int err;

  if (!data || data->user_id < 0 || !data->moves) {
    return EINVAL;
  }

  err = DisplaySubsystem_displays_get(&display_subsystem, data->display_id,
                                      &display);
  if (err) {
    logging_ops->log_err(module_id,
                         "Failed to get display for display_id %d: %s",
                         data->display_id, strerror(err));
    return err;
  }

  err = display->display(data);
  if (err) {
    logging_ops->log_err(module_id, "Display rendering failed for %s: %s",
                         display->display_name, strerror(err));
    return err;
  }

  logging_ops->log_info(module_id,
                        "Display rendering completed successfully for %s",
                        display->display_name);

  return 0;
}

static int display_add_display(struct DisplayDisplay *new_display) {
  int err;

  if (!new_display || !new_display->display || !new_display->display_name)
    return EINVAL;

  err = DisplaySubsystem_displays_append(&display_subsystem, *new_display);
  if (err) {
    logging_ops->log_err(module_id, "Unable to add display %s: %s",
                         new_display->display_name, strerror(err));
    return err;
  }

  return 0;
};

static int display_get_display_id(const char *display_name,
                                  int *id_placeholder) {
  struct DisplayDisplay *display;

  if (!display_name || !id_placeholder) {
    return EINVAL;
  }

  for (size_t i = 0; i < DisplaySubsystem_displays_length(&display_subsystem);
       i++) {
    DisplaySubsystem_displays_get(&display_subsystem, i, &display);

    if (std_lib_ops->are_str_eq((char *)display->display_name,
                                (char *)display_name)) {
      *id_placeholder = (int)i;
      return 0;
    }
  }

  *id_placeholder = -1;

  logging_ops->log_err(module_id, "Display name '%s' not found.", display_name);

  return ENOENT;
}

struct DisplaySubsystem *display_get_subsystem(void) {
  return &display_subsystem;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct DisplayPrivateOps {
  struct DisplaySubsystem *(*get_subsystem)(void);
};

static struct DisplayPrivateOps display_private_ops = {
    .get_subsystem = display_get_subsystem};

static struct DisplayOps display_ops = {
    .init = display_init,
    .display = display_display,
    .add_display = display_add_display,
    .get_display_id = display_get_display_id,
};

struct DisplayOps *get_display_ops(void) {
  return &display_ops;
};

struct DisplayPrivateOps *get_display_priv_ops(void) {
  return &display_private_ops;
};
