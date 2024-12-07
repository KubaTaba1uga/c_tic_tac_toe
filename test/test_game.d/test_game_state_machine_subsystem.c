/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <stddef.h>
#include <string.h>

// Tests framework
#include <unity.h>

// App's internal libs
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "init/init.h"
#include "utils/logging_utils.h"

// Mocks requirement
#include "game_state_machine_subsystem_wrapper.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_GAME_SM_SUBSYSTEM_TEST_REGISTRATIONS 100

struct GameSmSubsystem {
  size_t counter;
  struct GameSmSubsystemRegistrationData
      *registrations[MAX_GAME_SM_SUBSYSTEM_TEST_REGISTRATIONS];
};
static struct GameSmSubsystem test_data;
static int mock_state;

static struct GameSmSubsystemOps *gsm_sub_ops;
static struct LoggingUtilsOps *logging_ops;
static struct InitOps *init_ops;

static int mock_next_state(struct GameStateMachineInput input,
                           struct GameStateMachineState *state);
static struct GameSmSubsystem *mock_get_subsystem(void);
static void mock_functions(void);

/*******************************************************************************
 *    TESTS FRAMEWORK BOILERCODE
 ******************************************************************************/
void setUp() {
  gsm_sub_ops = get_game_sm_subsystem_ops();
  logging_ops = get_logging_utils_ops();
  init_ops = get_init_ops();

  // Disable game init and destroy
  init_game_reg.init = NULL;
  init_game_reg.destroy = NULL;
  init_ops->initialize_system();

  mock_functions();
}

void tearDown() {
  // Cleanup code if needed
  init_ops->destroy_system();
}

/*******************************************************************************
 *    TESTS
 ******************************************************************************/
// Test register_state_machine
void test_register_new_state_machine() {
  // Mock registration data
  struct GameSmSubsystemRegistrationData mock_registration_data_1 = {
      .id = "TestSM1", .priority = 1, .next_state = mock_next_state};

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1, test_data.registrations[0]);
  TEST_ASSERT_EQUAL_INT(1, test_data.counter);

  // Test if subsystem respects the max registration limit
  for (size_t i = 1; i < MAX_GAME_SM_SUBSYSTEM_TEST_REGISTRATIONS; i++) {
    gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  }
  TEST_ASSERT_EQUAL_INT(MAX_GAME_SM_SUBSYSTEM_TEST_REGISTRATIONS,
                        test_data.counter);

  // Try registering beyond the limit
  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(MAX_GAME_SM_SUBSYSTEM_TEST_REGISTRATIONS,
                        test_data.counter);
}

void test_register_positive_prioritized_state_machine() {
  struct GameSmSubsystemRegistrationData mock_registration_data_0 = {
      .id = "TestSM0", .priority = 0, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_1 = {
      .id = "TestSM1", .priority = 1, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_2 = {
      .id = "TestSM2", .priority = 2, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_3 = {
      .id = "TestSM3", .priority = 3, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_4 = {
      .id = "TestSM4", .priority = 4, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_5 = {
      .id = "TestSM5", .priority = 5, .next_state = mock_next_state};

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(1, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(2, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(3, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(4, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(5, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(6, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(7, test_data.counter);

  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1, test_data.registrations[0]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2, test_data.registrations[1]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3, test_data.registrations[2]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4, test_data.registrations[3]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5, test_data.registrations[4]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[5]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[6]);
}

void test_register_negative_prioritized_state_machine() {
  struct GameSmSubsystemRegistrationData mock_registration_data_0 = {
      .id = "TestSM0", .priority = 0, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_1 = {
      .id = "TestSM1", .priority = -1, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_2 = {
      .id = "TestSM2", .priority = -2, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_3 = {
      .id = "TestSM3", .priority = -3, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_4 = {
      .id = "TestSM4", .priority = -4, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_5 = {
      .id = "TestSM5", .priority = -5, .next_state = mock_next_state};

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(1, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(2, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(3, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(4, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(5, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(6, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(7, test_data.counter);

  for (size_t i = 0; i < test_data.counter; i++) {
    logging_ops->log_info("test", (char *)test_data.registrations[i]->id);
  }

  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[0]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[1]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5, test_data.registrations[2]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4, test_data.registrations[3]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3, test_data.registrations[4]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2, test_data.registrations[5]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1, test_data.registrations[6]);
}

void test_register_all_prioritized_state_machine_small() {
  struct GameSmSubsystemRegistrationData mock_registration_data_0 = {
      .id = "TestSM0", .priority = 0, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_1 = {
      .id = "TestSM1", .priority = 1, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_2 = {
      .id = "TestSM2", .priority = 2, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_3 = {
      .id = "TestSM-2", .priority = -2, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_4 = {
      .id = "TestSM-1", .priority = -1, .next_state = mock_next_state};

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(1, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(2, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(3, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(4, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(5, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(6, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(7, test_data.counter);

  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1, test_data.registrations[0]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2, test_data.registrations[1]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[2]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[3]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[4]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3, test_data.registrations[5]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4, test_data.registrations[6]);
}

void test_register_all_prioritized_state_machine_big() {
  struct GameSmSubsystemRegistrationData mock_registration_data_0 = {
      .id = "TestSM0", .priority = 0, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_1 = {
      .id = "TestSM1", .priority = 1, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_2 = {
      .id = "TestSM2", .priority = 2, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_3 = {
      .id = "TestSM3", .priority = 3, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_4 = {
      .id = "TestSM4", .priority = 4, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_5 = {
      .id = "TestSM-4", .priority = -4, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_6 = {
      .id = "TestSM-3", .priority = -3, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_7 = {
      .id = "TestSM-2", .priority = -2, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_8 = {
      .id = "TestSM-1", .priority = -1, .next_state = mock_next_state};

  gsm_sub_ops->register_state_machine(&mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(1, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(2, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_8);
  TEST_ASSERT_EQUAL_INT(3, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(4, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(5, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(6, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_7);
  TEST_ASSERT_EQUAL_INT(7, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(8, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(9, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(10, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_6);
  TEST_ASSERT_EQUAL_INT(11, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(12, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(13, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(14, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(15, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(16, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(17, test_data.counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(18, test_data.counter);

  /* for (size_t i = 0; i < test_data.counter; i++) { */
  /*   logging_ops_->log_info("test", */
  /*                              (char
   * *)test_data.registrations[i]->id); */
  /* } */

  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1, test_data.registrations[0]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2, test_data.registrations[1]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3, test_data.registrations[2]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4, test_data.registrations[3]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[4]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[5]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[6]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[7]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[8]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[9]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[10]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[11]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[12]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0, test_data.registrations[13]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5, test_data.registrations[14]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_6, test_data.registrations[15]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_7, test_data.registrations[16]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_8, test_data.registrations[17]);
}

void mock_functions(void) {
  struct GameSmSubsystemPrivateOps *priv_ops;

  // Prepare data
  test_data.counter = 0;
  mock_state = GameStatePlay;

  memset(test_data.registrations, 0, sizeof(test_data.registrations));

  // Mock functions
  priv_ops = gsm_sub_ops->private_ops;
  priv_ops->get_subsystem = mock_get_subsystem;
}

int mock_next_state(struct GameStateMachineInput input,
                    struct GameStateMachineState *state) {
  return mock_state;
}

struct GameSmSubsystem *mock_get_subsystem(void) { return &test_data; };
