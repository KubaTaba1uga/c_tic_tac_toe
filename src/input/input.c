/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// App's internal libs
#include "init/init.h"
#include "input/input.h"
#include "input/input_registration.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"
#include "utils/subsystem_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct InputSubsystem {
  subsystem_t subsystem;
};

typedef struct InputSubsystem *input_sys_t;

struct InputPrivateOps {
  int (*stop)(input_sys_t);
  int (*wait)(input_sys_t);
  int (*start)(input_sys_t);
  int (*init)(input_sys_t *);
  void (*destroy)(input_sys_t *);
  bool (*search_filter_id)(const char *, void *, void *);
  bool (*search_filter_all)(const char *, void *, void *);
  int (*register_module)(input_sys_t, struct InputRegistrationData *);
  int (*register_callback)(input_sys_t, char *, input_callback_func_t);
};

static const size_t subsystem_max = 10;
static struct StdLibUtilsOps *std_lib_ops;
static struct LoggingUtilsOps *logging_ops;
static struct InputSubsystem *input_subsystem;
static struct SubsystemUtilsOps *subsystem_ops;
static struct InputRegistrationOps *input_reg_ops;
static const char module_id[] = "input_subsystem";

static struct InputPrivateOps *input_priv_ops;
struct InputPrivateOps *get_input_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int input_init_system(void) {
  int err;
  input_reg_ops = get_input_reg_ops();
  logging_ops = get_logging_utils_ops();
  std_lib_ops = get_std_lib_utils_ops();
  input_priv_ops = get_input_priv_ops();
  subsystem_ops = get_subsystem_utils_ops();

  logging_ops->log_info(module_id, "Initializating subsystem");

  err = input_priv_ops->init(&input_subsystem);
  if (err) {
    return err;
  }

  return 0;
}

static void input_destroy_system(void) {
  input_priv_ops->destroy(&input_subsystem);
}

static int input_register_system(input_reg_t input_registration_data) {
  if (!input_registration_data)
    return EINVAL;

  logging_ops->log_info(module_id, "Registering %s into subsystem",
                        input_reg_ops->get_id(input_registration_data));

  return input_priv_ops->register_module(input_subsystem,
                                         input_registration_data);
}

static int input_register_callback_system(char *id,
                                          input_callback_func_t callback) {
  if (!id || !callback)
    return EINVAL;

  logging_ops->log_info(module_id, "Registering callback for %s", id);

  return input_priv_ops->register_callback(input_subsystem, id, callback);
};

static int input_start_system(void) {
  return input_priv_ops->start(input_subsystem);
}

static int input_stop_system(void) {
  return input_priv_ops->stop(input_subsystem);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int input_init(input_sys_t *input) {
  input_sys_t tmp_input;
  int err;

  if (!subsystem_ops)
    return ENODATA;

  tmp_input = malloc(sizeof(struct InputSubsystem));
  if (!tmp_input) {
    logging_ops->log_err(module_id,
                         "Unable to allocate memory for subsystem in input: %s",
                         strerror(ENOMEM));
    return ENOMEM;
  }

  err = subsystem_ops->init(&tmp_input->subsystem, module_id, subsystem_max);
  if (err) {
    logging_ops->log_err(module_id, "Unable to create subsystem in input: %s",
                         strerror(err));
    return err;
  }

  *input = tmp_input;

  return 0;
}

static void input_destroy(input_sys_t *input) {
  input_sys_t tmp_input;

  if (!subsystem_ops)
    return;

  if (!input || !*input)
    return;

  tmp_input = *input;

  subsystem_ops->destroy(&tmp_input->subsystem);

  *input = NULL;
}

static int
input_register_module(input_sys_t input,
                      struct InputRegistrationData *registration_data) {
  int err;

  if (!subsystem_ops || !logging_ops) {
    return ENODATA;
  }

  registration_data->callback = NULL;

  err = subsystem_ops->register_module(input->subsystem, registration_data->id,
                                       registration_data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to register %s in input: %s",
                         registration_data->id, strerror(err));
    return err;
  }

  logging_ops->log_info(module_id, "Registered %s", registration_data->id);

  return 0;
};

static bool input_subsystem_search_filter_id(const char *tmp_id, void *__,
                                             void *target_id) {
  if (!tmp_id || !target_id) {
    return false;
  }

  return std_lib_ops->are_str_eq((char *)tmp_id, (char *)target_id);
}

static int input_register_callback(input_sys_t input, char *id,
                                   input_callback_func_t callback) {
  struct InputRegistrationData *registration_data = NULL;
  module_search_t search_wrap;
  int err;

  if (!subsystem_ops || !logging_ops || !input_priv_ops) {
    return ENODATA;
  }

  err = subsystem_ops->init_search_module_wrapper(
      &search_wrap, id, input_priv_ops->search_filter_id,
      (void **)&registration_data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init search wrapper: %s",
                         strerror(err));
    goto out;
  }

  err = subsystem_ops->search_modules(input->subsystem, search_wrap);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to search for"
                         "%s: %s",
                         registration_data->id, strerror(err));
    goto cleanup;
  }

  if (!registration_data) {
    err = ENOENT;
    goto cleanup;
  }

  registration_data->callback = callback;

  logging_ops->log_info(module_id, "Callback set for %s",
                        registration_data->id);

  err = 0;

cleanup:
  subsystem_ops->destroy_search_module_wrapper(&search_wrap);
out:
  return err;
}

static bool input_subsystem_search_filter_start(const char *_, void *tmp,
                                                void *__) {
  if (!tmp)
    return false;

  struct InputRegistrationData *tmp_reg = tmp;

  return tmp_reg->start != NULL;
}

static int input_start(input_sys_t input) {
  struct InputRegistrationData *registration_data = NULL;
  module_search_t search_wrap;
  int err;

  if (!subsystem_ops || !logging_ops) {
    return ENODATA;
  }

  err = subsystem_ops->init_search_module_wrapper(
      &search_wrap, NULL, input_priv_ops->search_filter_start,
      (void **)&registration_data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init search wrapper: %s",
                         strerror(err));
    goto out;
  }

  do {
    err = subsystem_ops->search_modules(input->subsystem, search_wrap);
    if (err) {
      logging_ops->log_err(module_id,
                           "Unable to search for"
                           "%s: %s",
                           registration_data->id, strerror(err));
      goto cleanup;
    }
    if (!registration_data)
      break;

    err = registration_data->start();
    if (err) {
      goto cleanup;
    }

    logging_ops->log_info(module_id, "Started %s", registration_data->id);
  } while (true);

  err = 0;

  logging_ops->log_info(module_id, "Started all modules");

cleanup:
  subsystem_ops->destroy_search_module_wrapper(&search_wrap);
out:
  return err;
}

static bool input_subsystem_search_filter_wait(const char *_, void *tmp,
                                               void *__) {
  if (!tmp)
    return false;

  struct InputRegistrationData *tmp_reg = tmp;

  return tmp_reg->wait != NULL;
}

static int input_wait(input_sys_t input) {
  struct InputRegistrationData *registration_data = NULL;
  module_search_t search_wrap;
  int err;

  if (!subsystem_ops || !logging_ops) {
    return ENODATA;
  }

  err = subsystem_ops->init_search_module_wrapper(
      &search_wrap, NULL, input_priv_ops->search_filter_wait,
      (void **)&registration_data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init search wrapper: %s",
                         strerror(err));
    goto out;
  }

  do {
    err = subsystem_ops->search_modules(input->subsystem, search_wrap);
    if (err) {
      logging_ops->log_err(module_id,
                           "Unable to search for"
                           "%s: %s",
                           registration_data->id, strerror(err));
      goto cleanup;
    }
    if (!registration_data)
      break;

    registration_data->wait();

    logging_ops->log_info(module_id, "Waited for %s", registration_data->id);
  } while (true);

  err = 0;

  logging_ops->log_info(module_id, "Waited for all modules");

cleanup:
  subsystem_ops->destroy_search_module_wrapper(&search_wrap);
out:
  return err;
}

static bool input_subsystem_search_filter_stop(const char *_, void *tmp,
                                               void *___) {
  if (!tmp)
    return false;

  struct InputRegistrationData *tmp_reg = tmp;

  return tmp_reg->destroy != NULL;
}

void input_stop(input_sys_t input) {
  struct InputRegistrationData *registration_data = NULL;
  module_search_t search_wrap;
  int err;

  if (!subsystem_ops || !logging_ops) {
    return;
  }

  err = subsystem_ops->init_search_module_wrapper(
      &search_wrap, NULL, input_priv_ops->search_filter_destroy,
      (void **)&registration_data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init search wrapper: %s",
                         strerror(err));
    goto out;
  }

  do {
    err = subsystem_ops->search_modules(input->subsystem, search_wrap);
    if (err) {
      logging_ops->log_err(module_id,
                           "Unable to search for"
                           "%s: %s",
                           registration_data->id, strerror(err));
      goto cleanup;
    }
    if (!registration_data)
      break;

    registration_data->stop();
    registration_data->callback = NULL;

    logging_ops->log_info(module_id, "Stopped %s", registration_data->id);
  } while (true);

  err = 0;

  logging_ops->log_info(module_id, "Stopped all modules");

cleanup:
  subsystem_ops->destroy_search_module_wrapper(&search_wrap);
out:
  return;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_input_reg = {
    .id = module_id,
    .init = input_init_system,
    .destroy = input_destroy_system,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InputOps input_ops = {
    .register_module = input_register_system,
};

struct InputOps *get_input_ops(void) { return &input_ops; };

static struct InputPrivateOps input_priv_ops_ = {
    .wait = input_wait,
    .init = input_init,
    .destroy = input_destroy,
    .register_module = input_register_module,
    .start_no_blocking = input_start_no_blocking,
    .search_filter_id = input_subsystem_search_filter_id,
    .search_filter_all = input_subsystem_search_filter_all,
    .search_filter_callback = input_subsystem_search_filter_callback,

};

struct InputPrivateOps *get_input_priv_ops(void) { return &input_priv_ops_; }
