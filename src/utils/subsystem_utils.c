/*******************************************************************************
 * @file subsystem_utils.c
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
#include <asm-generic/errno.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// App's internal libs
#include "init/init.h"
#include "utils/array_utils.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"
#include "utils/subsystem_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct Subsystem {
  const char *id;
  array_t registrations;
};

struct Module {
  const char *id;
  void *private;
};

struct SubsystemUtilsPrivateOps {};

static struct ArrayUtilsOps *array_ops;
static struct StdLibUtilsOps *std_lib_ops;
static struct LoggingUtilsOps *logging_ops;
static const char module_id[] = "subsystem_utils_subsystem";

static struct SubsystemUtilsPrivateOps *subsystem_utils_priv_ops;
struct SubsystemUtilsPrivateOps *get_subsystem_utils_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int subsystem_utils_init_system(void) {
  array_ops = get_array_utils_ops();
  logging_ops = get_logging_utils_ops();
  std_lib_ops = get_std_lib_utils_ops();
  subsystem_utils_priv_ops = get_subsystem_utils_priv_ops();

  return 0;
}

static int subsystem_utils_init(subsystem_t *subsystem, const char *id,
                                const size_t max_registrations) {
  subsystem_t tmp_subsystem;
  int err;

  if (!logging_ops || !array_ops)
    return ENODATA;

  if (!subsystem || !id)
    return EINVAL;

  tmp_subsystem = malloc(sizeof(struct Subsystem));
  if (!tmp_subsystem) {
    logging_ops->log_err(module_id,
                         "Unable to create subsystem in '%s' subsystem: %s", id,
                         strerror(ENOMEM));
    return ENOMEM;
  }

  err = array_ops->init(&tmp_subsystem->registrations, max_registrations);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to create array in '%s' subsystem: %s", id,
                         strerror(err));
    return err;
  }

  tmp_subsystem->id = id;

  *subsystem = tmp_subsystem;

  return 0;
}

static void subsystem_utils_destroy(subsystem_t *subsystem) {
  subsystem_t tmp_subsystem_utils;

  if (!array_ops)
    return;

  if (!subsystem || !*subsystem)
    return;

  tmp_subsystem_utils = *subsystem;

  array_ops->destroy(&tmp_subsystem_utils->registrations);

  *subsystem = NULL;
}

static int subsystem_utils_register_module(subsystem_t subsystem,
                                           const char *module_id,
                                           void *module_data) {
  module_t module;
  int err;

  if (!array_ops || !logging_ops)
    return ENODATA;

  if (!subsystem || !module_id)
    return EINVAL;

  module = malloc(sizeof(struct Module));
  if (!module) {
    logging_ops->log_err(module_id, "Unable to create module '%s' in '%s': %s",
                         module->id, subsystem->id, strerror(ENOMEM));
    return ENOMEM;
  }

  err = array_ops->append(subsystem->registrations, module);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to register module '%s' in '%s': %s",
                         module->id, subsystem->id, strerror(err));
    return err;
  }

  return 0;
};

void *subsystem_utils_get_module_by_filter(
    subsystem_t subsystem, void *filter_data,
    bool (*filter_func)(const char *, void *, void *)) {
  module_t module;
  size_t i;

  if (!array_ops || !logging_ops || !std_lib_ops) {
    return NULL;
  }

  if (!subsystem || !filter_func) {
    return NULL;
  }

  for (i = 0; i < array_ops->get_length(subsystem->registrations); i++) {
    module = array_ops->get_element(subsystem->registrations, i);
    if (filter_func(module->id, module->private, filter_data)) {
      return module->private;
    }
  }

  logging_ops->log_err(
      module_id,
      "Unable to get module '%s' from '%s': No module matches filter function",
      module->id, subsystem->id);

  return NULL;
};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_subsystem_utils_reg = {
    .id = module_id,
    .init = subsystem_utils_init_system,
    .destroy = NULL,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct SubsystemUtilsPrivateOps subsystem_utils_priv_ops_ = {

};

struct SubsystemUtilsOps subsystem_utils_pub_ops = {
    .init = subsystem_utils_init,
    .destroy = subsystem_utils_destroy,
    .register_module = subsystem_utils_register_module};

struct SubsystemUtilsOps *get_subsystem_utils_ops(void) {
  return &subsystem_utils_pub_ops;
};

struct SubsystemUtilsPrivateOps *get_subsystem_utils_priv_ops(void) {
  return &subsystem_utils_priv_ops_;
}
