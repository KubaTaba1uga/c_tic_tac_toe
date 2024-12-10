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
#include <asm-generic/errno.h>
#include <errno.h>
#include <stdbool.h>
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

struct ModuleSearchWrapper {
  bool (*filter_func_wrap)(const char *, void *, void *);
  array_search_t search_wrap;
  void *filter_data_wrap;
};

struct SubsystemUtilsPrivateOps {
  bool (*filter_func)(module_t, module_search_t);
};

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

static int subsystem_utils_init_search_module_wrapper(
    module_search_t *search_wrap, void *filter_data,
    bool (*filter_func)(const char *, void *, void *),
    void **result_placeholder) {
  module_search_t tmp_wrap;
  int err;

  if (!array_ops || !subsystem_utils_priv_ops) {
    return ENODATA;
  }

  if (!search_wrap || !filter_data || !filter_func || !result_placeholder)
    return EINVAL;

  tmp_wrap = malloc(sizeof(struct ModuleSearchWrapper));
  if (!tmp_wrap) {
    return ENOMEM;
  }

  tmp_wrap->filter_func_wrap = filter_func;
  tmp_wrap->filter_data_wrap = filter_data;

  err = array_ops->init_search_wrapper(
      &tmp_wrap->search_wrap, tmp_wrap,
      (bool (*)(void *, void *))subsystem_utils_priv_ops->filter_func,
      result_placeholder);

  if (err) {
    logging_ops->log_err(module_id, "Unable to init array search wrapper.");
    return err;
  }

  *search_wrap = tmp_wrap;

  return 0;
};

static void
subsystem_utils_destroy_search_module_wrapper(module_search_t *search_wrap) {
  module_search_t tmp_wrap;

  if (!array_ops)
    return;

  if (!search_wrap)
    return;

  tmp_wrap = *search_wrap;

  array_ops->destroy_search_wrapper(&tmp_wrap->search_wrap);
  free(tmp_wrap);

  *search_wrap = NULL;
};

static int subsystem_utils_search_modules(subsystem_t subsystem,
                                          module_search_t search_data) {
  return array_ops->search_elements(subsystem->registrations,
                                    search_data->search_wrap);
}

module_search_enum_t
subsystem_utils_get_state_search_module_wrapper(module_search_t search_wrap) {
  if (!array_ops)
    return ARRAY_SEARCH_STATE_INVALID;

  return array_ops->get_state_search_wrapper(search_wrap->search_wrap);
};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static bool subsystem_utils_filter_search_wrapper(module_t module,
                                                  module_search_t search_data) {
  if (!module || !search_data) {
    logging_ops->log_err(module_id, "Invalid module data, returning false from "
                                    "subsystem_utils_filter_search_wrapper");
    return false;
  }

  return search_data->filter_func_wrap(module->id, module->private,
                                       search_data->filter_data_wrap);
};

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
    .filter_func = subsystem_utils_filter_search_wrapper,
};

struct SubsystemUtilsOps subsystem_utils_pub_ops = {
    .init = subsystem_utils_init,
    .destroy = subsystem_utils_destroy,
    .register_module = subsystem_utils_register_module,
    .search_modules = subsystem_utils_search_modules,
    .init_search_module_wrapper = subsystem_utils_init_search_module_wrapper,
    .destroy_search_module_wrapper =
        subsystem_utils_destroy_search_module_wrapper,
    .get_state_search_module_wrapper =
        subsystem_utils_get_state_search_module_wrapper};

struct SubsystemUtilsOps *get_subsystem_utils_ops(void) {
  return &subsystem_utils_pub_ops;
};

struct SubsystemUtilsPrivateOps *get_subsystem_utils_priv_ops(void) {
  return &subsystem_utils_priv_ops_;
}
