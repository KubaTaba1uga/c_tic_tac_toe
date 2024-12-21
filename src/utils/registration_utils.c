/*******************************************************************************
 * @file registration_utils.c
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
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// App's internal libs
#include "init/init.h"
#include "utils/array_utils.h"
#include "utils/logging_utils.h"
#include "utils/registration_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static struct ArrayUtilsOps *array_ops;
static struct StdLibUtilsOps *std_lib_ops;
static struct LoggingUtilsOps *logging_ops;

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int registration_utils_init_system(void) {
  array_ops = get_array_utils_ops();
  logging_ops = get_logging_utils_ops();
  std_lib_ops = get_std_lib_utils_ops();

  return 0;
}

static int registration_utils_registrar_init(struct Registrar *registrar,
                                             char *display_name,
                                             const size_t max_regs_size) {
  int err;

  if (!registrar || !display_name) {
    return EINVAL;
  }

  memset(registrar, 0, sizeof(struct Registrar));

  strncpy(registrar->display_name, display_name,
          REGISTRATION_DISPLAY_NAME_MAX - 1);

  err = array_ops->init(&registrar->registrations, max_regs_size);
  if (err) {
    return err;
  }

  return 0;
}

static void registration_utils_registrar_destroy(struct Registrar *registrar) {
  if (!registrar) {
    return;
  }

  array_ops->destroy(&registrar->registrations);
};

static int
registration_utils_registration_init(struct Registration *registration,
                                     char *display_name, void *private) {
  if (!registration || !display_name) {
    return EINVAL;
  }

  memset(registration->display_name, 0, REGISTRATION_DISPLAY_NAME_MAX);
  strncpy(registration->display_name, display_name,
          REGISTRATION_DISPLAY_NAME_MAX);

  registration->private = private;

  return 0;
}

static int
registration_utils_register_input_init(struct RegisterInput *input,
                                       struct Registrar *registrar,
                                       struct Registration *registration) {
  if (!input || !registrar || !registration) {
    return EINVAL;
  }

  input->registrar = registrar;
  input->registration = registration;

  return 0;
}

static int registration_utils_register_module(struct RegisterInput input,
                                              struct RegisterOutput *output) {
  int err;

  if (!output) {
    return EINVAL;
  }

  // Set default output values
  output->registration_id = -1;

  err = array_ops->append(&input.registrar->registrations, input.registration);
  if (err) {
    return err;
  }

  output->registration_id = input.registrar->registrations.length - 1;

  return 0;
};

static int
registration_utils_get_registration(struct GetRegistrationInput input,
                                    struct GetRegistrationOutput *output) {
  int err;
  if (!output) {
    return EINVAL;
  }

  if (input.registration_id < 0 ||
      input.registration_id >= input.registrar->registrations.length) {
    return ENOENT;
  }

  err = array_ops->get_element(input.registrar->registrations,
                               input.registration_id,
                               (void **)&output->registration);
  if (err) {
    return err;
  }

  return 0;
};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_registration_utils_reg = {
    .id = __FILE__,
    .init = registration_utils_init_system,
    .destroy = NULL,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/

struct RegistrationUtilsOps registration_utils_pub_ops = {
    .init = registration_utils_registrar_init,
    .destroy = registration_utils_registrar_destroy,
    .registration_init = registration_utils_registration_init,
    .register_input_init = registration_utils_register_input_init,
    .register_module = registration_utils_register_module,
    .get_registration = registration_utils_get_registration,
};

struct RegistrationUtilsOps *get_registration_utils_ops(void) {
  return &registration_utils_pub_ops;
};
