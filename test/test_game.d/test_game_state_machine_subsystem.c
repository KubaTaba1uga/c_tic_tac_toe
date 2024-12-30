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
static struct GameSmSubsystem test_data;
static int mock_state;

static struct GameSmSubsystemOps *gsm_sub_ops;
static struct LoggingUtilsOps *logging_ops;
static struct InitOps *init_ops;

static int mock_next_state(struct GameStateMachineInput input,
                           struct GameStateMachineState *state);
static struct GameSmSubsystem *mock_get_subsystem(void);

/*******************************************************************************
 *    TESTS FRAMEWORK BOILERCODE
 ******************************************************************************/
void setUp() {
  struct GameSmSubsystemPrivateOps *priv_ops;

  // Mock functions
  gsm_sub_ops = get_game_sm_subsystem_ops();
  logging_ops = get_logging_utils_ops();
  init_ops = get_init_ops();
  priv_ops = get_gsm_sub_private_ops();

  GameSmSubsystem_mini_machines_init(&test_data);
  mock_state = GameStatePlay;
  priv_ops->get_subsystem = mock_get_subsystem;

  TEST_ASSERT_EQUAL_INT(0, init_ops->initialize());
}

void tearDown() { init_ops->destroy(); }

/*******************************************************************************
 *    TESTS
 ******************************************************************************/
// Test add_mini_state_machine
void test_register_new_state_machine() {
  // Mock registration data
  struct MiniGameStateMachine mock_registration_data_1 = {
      .display_name = "TestSM1", .priority = 1, .next_state = mock_next_state};
  int err;

  err = gsm_sub_ops->add_mini_state_machine(mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_1,
                           &test_data.mini_machines[0],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_INT(1, GameSmSubsystem_mini_machines_length(&test_data));

  // Test if subsystem respects the max registration limit
  for (size_t i = 1; i < GAME_SM_MINI_MACHINES_MAX; i++) {
    gsm_sub_ops->add_mini_state_machine(mock_registration_data_1);
    TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_1,
                             &test_data.mini_machines[i],
                             sizeof(struct MiniGameStateMachine));
  }

  TEST_ASSERT_EQUAL_INT(GAME_SM_MINI_MACHINES_MAX,
                        GameSmSubsystem_mini_machines_length(&test_data));

  // Try registering beyond the limit
  err = gsm_sub_ops->add_mini_state_machine(mock_registration_data_1);
  TEST_ASSERT_NOT_EQUAL(0, err);
  TEST_ASSERT_EQUAL_INT(GAME_SM_MINI_MACHINES_MAX,
                        GameSmSubsystem_mini_machines_length(&test_data));
}

/* void test_register_positive_prioritized_state_machine() { */
/*   struct MiniGameStateMachine mock_registration_data_0 = { */
/*       .id = "TestSM0", .priority = 0, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_1 = { */
/*       .id = "TestSM1", .priority = 1, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_2 = { */
/*       .id = "TestSM2", .priority = 2, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_3 = { */
/*       .id = "TestSM3", .priority = 3, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_4 = { */
/*       .id = "TestSM4", .priority = 4, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_5 = { */
/*       .id = "TestSM5", .priority = 5, .next_state = mock_next_state}; */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(1, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_3); */
/*   TEST_ASSERT_EQUAL_INT(2, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_1); */
/*   TEST_ASSERT_EQUAL_INT(3, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_4); */
/*   TEST_ASSERT_EQUAL_INT(4, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(5, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_2); */
/*   TEST_ASSERT_EQUAL_INT(6, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_5); */
/*   TEST_ASSERT_EQUAL_INT(7, test_data.counter); */

/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
 * test_data.mini_machines[0]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2,
 * test_data.mini_machines[1]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3,
 * test_data.mini_machines[2]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4,
 * test_data.mini_machines[3]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5,
 * test_data.mini_machines[4]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[5]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[6]); */
/* } */

/* void test_register_negative_prioritized_state_machine() { */
/*   struct MiniGameStateMachine mock_registration_data_0 = { */
/*       .id = "TestSM0", .priority = 0, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_1 = { */
/*       .id = "TestSM1", .priority = -1, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_2 = { */
/*       .id = "TestSM2", .priority = -2, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_3 = { */
/*       .id = "TestSM3", .priority = -3, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_4 = { */
/*       .id = "TestSM4", .priority = -4, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_5 = { */
/*       .id = "TestSM5", .priority = -5, .next_state = mock_next_state}; */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(1, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_3); */
/*   TEST_ASSERT_EQUAL_INT(2, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_1); */
/*   TEST_ASSERT_EQUAL_INT(3, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_4); */
/*   TEST_ASSERT_EQUAL_INT(4, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(5, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_2); */
/*   TEST_ASSERT_EQUAL_INT(6, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_5); */
/*   TEST_ASSERT_EQUAL_INT(7, test_data.counter); */

/*   for (size_t i = 0; i < test_data.counter; i++) { */
/*     logging_ops->log_info("test", (char *)test_data.mini_machines[i]->id); */
/*   } */

/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[0]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[1]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5,
 * test_data.mini_machines[2]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4,
 * test_data.mini_machines[3]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3,
 * test_data.mini_machines[4]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2,
 * test_data.mini_machines[5]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
 * test_data.mini_machines[6]); */
/* } */

/* void test_register_all_prioritized_state_machine_small() { */
/*   struct MiniGameStateMachine mock_registration_data_0 = { */
/*       .id = "TestSM0", .priority = 0, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_1 = { */
/*       .id = "TestSM1", .priority = 1, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_2 = { */
/*       .id = "TestSM2", .priority = 2, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_3 = { */
/*       .id = "TestSM-2", .priority = -2, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_4 = { */
/*       .id = "TestSM-1", .priority = -1, .next_state = mock_next_state}; */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(1, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_3); */
/*   TEST_ASSERT_EQUAL_INT(2, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_1); */
/*   TEST_ASSERT_EQUAL_INT(3, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_4); */
/*   TEST_ASSERT_EQUAL_INT(4, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(5, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_2); */
/*   TEST_ASSERT_EQUAL_INT(6, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(7, test_data.counter); */

/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
 * test_data.mini_machines[0]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2,
 * test_data.mini_machines[1]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[2]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[3]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[4]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3,
 * test_data.mini_machines[5]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4,
 * test_data.mini_machines[6]); */
/* } */

/* void test_register_all_prioritized_state_machine_big() { */
/*   struct MiniGameStateMachine mock_registration_data_0 = { */
/*       .id = "TestSM0", .priority = 0, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_1 = { */
/*       .id = "TestSM1", .priority = 1, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_2 = { */
/*       .id = "TestSM2", .priority = 2, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_3 = { */
/*       .id = "TestSM3", .priority = 3, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_4 = { */
/*       .id = "TestSM4", .priority = 4, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_5 = { */
/*       .id = "TestSM-4", .priority = -4, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_6 = { */
/*       .id = "TestSM-3", .priority = -3, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_7 = { */
/*       .id = "TestSM-2", .priority = -2, .next_state = mock_next_state}; */
/*   struct MiniGameStateMachine mock_registration_data_8 = { */
/*       .id = "TestSM-1", .priority = -1, .next_state = mock_next_state}; */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_4); */
/*   TEST_ASSERT_EQUAL_INT(1, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(2, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_8); */
/*   TEST_ASSERT_EQUAL_INT(3, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(4, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_3); */
/*   TEST_ASSERT_EQUAL_INT(5, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(6, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_7); */
/*   TEST_ASSERT_EQUAL_INT(7, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(8, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_2); */
/*   TEST_ASSERT_EQUAL_INT(9, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(10, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_6); */
/*   TEST_ASSERT_EQUAL_INT(11, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(12, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_1); */
/*   TEST_ASSERT_EQUAL_INT(13, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(14, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_5); */
/*   TEST_ASSERT_EQUAL_INT(15, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(16, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(17, test_data.counter); */

/*   gsm_sub_ops->add_mini_state_machine(&mock_registration_data_0); */
/*   TEST_ASSERT_EQUAL_INT(18, test_data.counter); */

/*   /\* for (size_t i = 0; i < test_data.counter; i++) { *\/ */
/*   /\*   logging_ops_->log_info("test", *\/ */
/*   /\*                              (char */
/*    * *)test_data.mini_machines[i]->id); *\/ */
/*   /\* } *\/ */

/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_1,
 * test_data.mini_machines[0]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_2,
 * test_data.mini_machines[1]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_3,
 * test_data.mini_machines[2]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_4,
 * test_data.mini_machines[3]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[4]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[5]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[6]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[7]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[8]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[9]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[10]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[11]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[12]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_0,
 * test_data.mini_machines[13]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_5,
 * test_data.mini_machines[14]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_6,
 * test_data.mini_machines[15]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_7,
 * test_data.mini_machines[16]); */
/*   TEST_ASSERT_EQUAL_PTR(&mock_registration_data_8,
 * test_data.mini_machines[17]); */
/* } */

int mock_next_state(struct GameStateMachineInput input,
                    struct GameStateMachineState *state) {
  return mock_state;
}

struct GameSmSubsystem *mock_get_subsystem(void) { return &test_data; };
