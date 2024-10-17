#include <stddef.h>
#include <unity.h>

#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game_state_machine_subsystem_wrapper.h"

#include "init/init.h"
#include "utils/logging_utils.h"

struct LoggingUtilsOps *logging_ops_;
struct GameSmSubsystemOps *gsm_sub_ops;

// Mock data for testing
struct GameStateMachineState mock_state = {.current_state = 0};
struct GameStateMachineState mock_new_state = {.current_state = 1};
struct GameStateMachineInput mock_input = {};
size_t mock_counter;

// Mock next_state function
int mock_next_state(struct GameStateMachineInput input,
                    struct GameStateMachineState *state) {
  return 0;
}

// Mock next_state function
size_t *mock_get_counter(void) { return &mock_counter; }

// Test initialization
void setUp() {
  char *disabled_modules_ids[] = {"game"};
  struct InitOps *init_ops;

  gsm_sub_ops = get_game_sm_subsystem_ops();
  init_ops = get_init_ops();

  init_ops->initialize_system_with_disabled_modules(
      sizeof(disabled_modules_ids) / sizeof(char *), disabled_modules_ids);

  logging_ops_ = get_logging_utils_ops();

  mock_counter = 0;
}

void tearDown() {
  // Cleanup code if needed
  struct InitOps *init_ops;
  init_ops = get_init_ops();

  init_ops->destroy_system();
}

// Test register_state_machine
void test_register_new_state_machine() {
  // Mock registration data
  struct GameSmSubsystemRegistrationData mock_registration_data_1 = {
      .id = "TestSM1", .priority = 1, .next_state = mock_next_state};

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
                        game_sm_subsystem.registrations[0]);
  TEST_ASSERT_EQUAL_INT(1, mock_counter);

  // Test if subsystem respects the max registration limit
  for (size_t i = 1; i < MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS; i++) {
    gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  }
  TEST_ASSERT_EQUAL_INT(MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS, mock_counter);

  // Try registering beyond the limit
  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS, mock_counter);
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
  TEST_ASSERT_EQUAL_INT(1, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(2, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(3, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(4, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(5, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(6, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(7, mock_counter);

  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
                        game_sm_subsystem.registrations[0]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2,
                        game_sm_subsystem.registrations[1]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3,
                        game_sm_subsystem.registrations[2]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4,
                        game_sm_subsystem.registrations[3]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5,
                        game_sm_subsystem.registrations[4]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[5]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[6]);
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
  TEST_ASSERT_EQUAL_INT(1, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(2, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(3, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(4, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(5, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(6, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(7, mock_counter);

  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[0]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[1]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5,
                        game_sm_subsystem.registrations[2]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4,
                        game_sm_subsystem.registrations[3]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3,
                        game_sm_subsystem.registrations[4]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2,
                        game_sm_subsystem.registrations[5]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
                        game_sm_subsystem.registrations[6]);
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
  TEST_ASSERT_EQUAL_INT(1, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(2, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(3, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(4, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(5, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(6, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(7, mock_counter);

  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
                        game_sm_subsystem.registrations[0]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2,
                        game_sm_subsystem.registrations[1]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[2]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[3]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[4]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3,
                        game_sm_subsystem.registrations[5]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4,
                        game_sm_subsystem.registrations[6]);
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
  TEST_ASSERT_EQUAL_INT(1, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(2, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_8);
  TEST_ASSERT_EQUAL_INT(3, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(4, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(5, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(6, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_7);
  TEST_ASSERT_EQUAL_INT(7, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(8, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(9, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(10, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_6);
  TEST_ASSERT_EQUAL_INT(11, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(12, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(13, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(14, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(15, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(16, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(17, mock_counter);

  gsm_sub_ops->register_state_machine(&mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(18, mock_counter);

  /* for (size_t i = 0; i < mock_counter; i++) { */
  /*   logging_ops_->log_info("test", */
  /*                              (char
   * *)game_sm_subsystem.registrations[i]->id); */
  /* } */

  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
                        game_sm_subsystem.registrations[0]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2,
                        game_sm_subsystem.registrations[1]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3,
                        game_sm_subsystem.registrations[2]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4,
                        game_sm_subsystem.registrations[3]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[4]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[5]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[6]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[7]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[8]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[9]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[10]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[11]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[12]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
                        game_sm_subsystem.registrations[13]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5,
                        game_sm_subsystem.registrations[14]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_6,
                        game_sm_subsystem.registrations[15]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_7,
                        game_sm_subsystem.registrations[16]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_8,
                        game_sm_subsystem.registrations[17]);
}
