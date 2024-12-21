#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unity.h>

// Include the header files
#include "init/init.h"
#include "utils/array_utils.h"
#include "utils/logging_utils.h"
#include "utils/registration_utils.h"
#include "utils/std_lib_utils.h"

// Mock constants
#define MAX_REGISTRATIONS 10
#define TEST_REGISTRAR_NAME "TestRegistrar"
#define TEST_REGISTRATION_NAME "TestRegistration"
#define TEST_PRIVATE_DATA ((void *)0xdeadbeef)

// Mock dependencies
static struct ArrayUtilsOps *array_ops;
static struct LoggingUtilsOps *logging_ops;
static struct StdLibUtilsOps *std_lib_ops;

static struct RegistrationUtilsOps *registration_ops;
static struct Registrar test_registrar;

// Test setup
void setUp() {
  int err;

  // Get utility ops
  registration_ops = get_registration_utils_ops();
  array_ops = get_array_utils_ops();
  logging_ops = get_logging_utils_ops();
  std_lib_ops = get_std_lib_utils_ops();

  // Initialize the registration system
  err = init_registration_utils_reg.init();
  TEST_ASSERT_EQUAL_INT(0, err);

  // Initialize the test registrar
  err = registration_ops->init(&test_registrar, TEST_REGISTRAR_NAME,
                               MAX_REGISTRATIONS);
  TEST_ASSERT_EQUAL_INT(0, err);
}

// Test teardown
void tearDown() { registration_ops->destroy(&test_registrar); }

// Test registrar initialization
void test_registration_utils_registrar_init() {
  struct Registrar registrar;
  int result = registration_ops->init(&registrar, TEST_REGISTRAR_NAME,
                                      MAX_REGISTRATIONS);

  TEST_ASSERT_EQUAL_INT(0, result);
  TEST_ASSERT_EQUAL_STRING(TEST_REGISTRAR_NAME, registrar.display_name);
  TEST_ASSERT_EQUAL_INT(0, registrar.registrations.length);

  registration_ops->destroy(&registrar);
}

// Test registrar destruction
void test_registration_utils_registrar_destroy() {
  struct Registrar registrar;
  registration_ops->init(&registrar, TEST_REGISTRAR_NAME, MAX_REGISTRATIONS);

  registration_ops->destroy(&registrar);
  // No crash is the expected outcome for this test
}

// Test registration initialization
void test_registration_utils_registration_init() {
  struct Registration registration;
  int result = registration_ops->registration_init(
      &registration, TEST_REGISTRATION_NAME, TEST_PRIVATE_DATA);

  TEST_ASSERT_EQUAL_INT(0, result);
  TEST_ASSERT_EQUAL_STRING(TEST_REGISTRATION_NAME, registration.display_name);
  TEST_ASSERT_EQUAL_PTR(TEST_PRIVATE_DATA, registration.private);
}

// Test registering a module
void test_registration_utils_register_module() {
  struct Registration registration;
  struct RegisterInput input;
  struct RegisterOutput output;

  registration_ops->registration_init(&registration, TEST_REGISTRATION_NAME,
                                      TEST_PRIVATE_DATA);
  registration_ops->register_input_init(&input, &test_registrar, &registration);

  int result = registration_ops->register_module(input, &output);

  TEST_ASSERT_EQUAL_INT(0, result);
  TEST_ASSERT_EQUAL_INT(0, output.registration_id);
  TEST_ASSERT_EQUAL_INT(1, test_registrar.registrations.length);
}

// Test getting a registered module
void test_registration_utils_get_registration() {
  struct Registration registration;
  struct RegisterInput input;
  struct RegisterOutput output;

  struct GetRegistrationInput get_input;
  struct GetRegistrationOutput get_output;

  registration_ops->registration_init(&registration, TEST_REGISTRATION_NAME,
                                      TEST_PRIVATE_DATA);
  registration_ops->register_input_init(&input, &test_registrar, &registration);
  registration_ops->register_module(input, &output);

  get_input.registrar = &test_registrar;
  get_input.registration_id = output.registration_id;

  int result = registration_ops->get_registration(get_input, &get_output);

  TEST_ASSERT_EQUAL_INT(0, result);
  TEST_ASSERT_EQUAL_STRING(TEST_REGISTRATION_NAME,
                           get_output.registration->display_name);
  TEST_ASSERT_EQUAL_PTR(TEST_PRIVATE_DATA, get_output.registration->private);
}

// Test registering multiple modules
void test_registration_utils_register_multiple_modules() {
  struct Registration registration1, registration2;
  struct RegisterInput input1, input2;
  struct RegisterOutput output1, output2;

  registration_ops->registration_init(&registration1, "Module1",
                                      TEST_PRIVATE_DATA);
  registration_ops->registration_init(&registration2, "Module2",
                                      TEST_PRIVATE_DATA);

  registration_ops->register_input_init(&input1, &test_registrar,
                                        &registration1);
  registration_ops->register_input_init(&input2, &test_registrar,
                                        &registration2);

  int result1 = registration_ops->register_module(input1, &output1);
  int result2 = registration_ops->register_module(input2, &output2);

  TEST_ASSERT_EQUAL_INT(0, result1);
  TEST_ASSERT_EQUAL_INT(0, result2);
  TEST_ASSERT_EQUAL_INT(2, test_registrar.registrations.length);
}
