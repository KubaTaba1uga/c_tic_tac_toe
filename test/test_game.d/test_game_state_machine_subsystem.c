
#include <unity.h>

#include "game/game_state_machine/game_sm_subsystem.c"

#include "game/game_state_machine/game_state_machine.h"

#include "utils/logging_utils.h"

// Mock data for testing
struct GameStateMachineState mock_state = {.current_state = 0};
struct GameStateMachineState mock_new_state = {.current_state = 1};
struct GameStateMachineInput mock_input = {};

// Mock next_state function
struct GameStateMachineState
mock_next_state(struct GameStateMachineInput input,
                struct GameStateMachineState state) {
  return mock_new_state;
}

// Mock registration data
struct GameSmSubsystemRegistrationData mock_registration_data_1 = {
    .id = "TestSM1", .priority = 1, .next_state = mock_next_state};

struct GameSmSubsystemRegistrationData mock_registration_data_2 = {
    .id = "TestSM2", .priority = -1, .next_state = mock_next_state};

// Test initialization
void setUp() {
  logging_utils_ops.init_loggers();
  game_sm_subsystem.count = 0;
}

void tearDown() {
  // Cleanup code if needed
  logging_utils_ops.destroy_loggers();
}

// Test register_state_machine
void test_register_new_state_machine() {
  register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
                        game_sm_subsystem.registrations[0]);
  TEST_ASSERT_EQUAL_INT(1, game_sm_subsystem.count);

  // Test if subsystem respects the max registration limit
  for (size_t i = 1; i < MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS; i++) {
    register_state_machine(&mock_registration_data_1);
  }
  TEST_ASSERT_EQUAL_INT(MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS,
                        game_sm_subsystem.count);

  // Try registering beyond the limit
  register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS,
                        game_sm_subsystem.count);
}

void test_register_positive_prioritized_state_machine() {
  struct GameSmSubsystemRegistrationData mock_registration_data_3 = {
      .id = "TestSM3", .priority = 0, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_4 = {
      .id = "TestSM4", .priority = 0, .next_state = mock_next_state};
  struct GameSmSubsystemRegistrationData mock_registration_data_5 = {
      .id = "TestSM5", .priority = 3, .next_state = mock_next_state};

  register_state_machine(&mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(1, game_sm_subsystem.count);

  register_state_machine(&mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(2, game_sm_subsystem.count);

  register_state_machine(&mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(3, game_sm_subsystem.count);

  register_state_machine(&mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(4, game_sm_subsystem.count);

  for (size_t i = 0; i < game_sm_subsystem.count; i++) {
    logging_utils_ops.log_info("test",
                               (char *)game_sm_subsystem.registrations[i]->id);
  }

  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
                        game_sm_subsystem.registrations[0]);
  TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5,
                        game_sm_subsystem.registrations[1]);
}
