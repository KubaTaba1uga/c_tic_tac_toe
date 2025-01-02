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
#include "game/game_state_machine/mini_state_machines/display_mini_machine.h"
#include "game/game_state_machine/mini_state_machines/moves_cleanup_mini_machine.h"
#include "game/game_state_machine/mini_state_machines/quit_mini_machine.h"
#include "game/game_state_machine/mini_state_machines/user_move_mini_machine.h"
#include "game/game_state_machine/mini_state_machines/user_turn_mini_machine.h"
#include "game/game_state_machine/mini_state_machines/win_mini_machine.h"
#include "init/init.h"
#include "utils/logging_utils.h"

// Mocks requirement
#include "game_state_machine_subsystem_wrapper.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static struct GameSmSubsystem test_data = {0};
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
  struct GameSmUserMoveModuleOps *user_move_ops;
  struct GameSmUserTurnModuleOps *user_turn_ops;
  struct GameSmQuitModuleOps *quit_ops;
  struct GameSmCleanLastMoveModuleOps *clean_last_move_ops;
  struct GameSmDisplayModuleOps *display_ops;
  struct GameSmWinModuleOps *win_ops;
  // Mock functions
  gsm_sub_ops = get_game_sm_subsystem_ops();
  logging_ops = get_logging_utils_ops();
  init_ops = get_init_ops();
  priv_ops = get_gsm_sub_private_ops();
  user_move_ops = get_game_sm_user_move_module_ops();
  user_turn_ops = get_game_sm_user_turn_module_ops();
  quit_ops = get_game_sm_quit_module_ops();
  clean_last_move_ops = get_game_sm_clean_last_move_module_ops();
  display_ops = get_game_sm_display_module_ops();
  win_ops = get_game_sm_win_module_ops();
  user_move_ops->init = NULL;
  user_turn_ops->init = NULL;
  quit_ops->init = NULL;
  clean_last_move_ops->init = NULL;
  display_ops->init = NULL;
  win_ops->init = NULL;

  priv_ops->get_subsystem = mock_get_subsystem;
  GameSmSubsystem_mini_machines_init(&test_data);
  mock_state = GameStatePlay;

  /* memset(&test_data, 0, sizeof(struct GameSmSubsystem)); */
  TEST_ASSERT_EQUAL_INT(0, init_ops->initialize());
}

void tearDown() { init_ops->destroy(); }

/*******************************************************************************
 *    TESTS
 ******************************************************************************/
// Test add_mini_state_machine
void test_register_new_state_machine() {
  int err;
  struct MiniGameStateMachine mock_registration_data_1 = {0}; //
  mock_registration_data_1.display_name = "TestSM1";
  mock_registration_data_1.priority = 1;
  mock_registration_data_1.next_state = mock_next_state;

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

void test_register_positive_prioritized_state_machine() {
  struct MiniGameStateMachine mock_registration_data_0 = {0};
  mock_registration_data_0.display_name = "TestSM0";
  mock_registration_data_0.priority = 0;
  mock_registration_data_0.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_1 = {0};
  mock_registration_data_1.display_name = "TestSM1";
  mock_registration_data_1.priority = 1;
  mock_registration_data_1.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_2 = {0};
  mock_registration_data_2.display_name = "TestSM2";
  mock_registration_data_2.priority = 2;
  mock_registration_data_2.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_3 = {0};
  mock_registration_data_3.display_name = "TestSM3";
  mock_registration_data_3.priority = 3;
  mock_registration_data_3.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_4 = {0};
  mock_registration_data_4.display_name = "TestSM4";
  mock_registration_data_4.priority = 4;
  mock_registration_data_4.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_5 = {0};
  mock_registration_data_5.display_name = "TestSM5";
  mock_registration_data_5.priority = 5;
  mock_registration_data_5.next_state = mock_next_state;

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(1, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(2, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(3, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(4, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(5, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(6, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(7, test_data.mini_machines_offset);

  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_1,
                           &test_data.mini_machines[0],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_2,
                           &test_data.mini_machines[1],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_3,
                           &test_data.mini_machines[2],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_4,
                           &test_data.mini_machines[3],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_5,
                           &test_data.mini_machines[4],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[5],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[6],
                           sizeof(struct MiniGameStateMachine));
}

void test_register_negative_prioritized_state_machine() {
  struct MiniGameStateMachine mock_registration_data_0 = {0};
  mock_registration_data_0.display_name = "TestSM0";
  mock_registration_data_0.priority = 0;
  mock_registration_data_0.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_1 = {0};
  mock_registration_data_1.display_name = "TestSM1";
  mock_registration_data_1.priority = -1;
  mock_registration_data_1.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_2 = {0};
  mock_registration_data_2.display_name = "TestSM2";
  mock_registration_data_2.priority = -2;
  mock_registration_data_2.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_3 = {0};
  mock_registration_data_3.display_name = "TestSM3";
  mock_registration_data_3.priority = -3;
  mock_registration_data_3.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_4 = {0};
  mock_registration_data_4.display_name = "TestSM4";
  mock_registration_data_4.priority = -4;
  mock_registration_data_4.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_5 = {0};
  mock_registration_data_5.display_name = "TestSM5";
  mock_registration_data_5.priority = -5;
  mock_registration_data_5.next_state = mock_next_state;

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(1, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(2, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(3, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(4, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(5, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(6, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(7, test_data.mini_machines_offset);

  /* for (size_t i = 0; i < test_data.mini_machines_offset; i++) { */
  /*   logging_ops->log_info("test", (char *)test_data.mini_machines[i]->id); */
  /* } */

  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[0],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[1],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_5,
                           &test_data.mini_machines[2],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_4,
                           &test_data.mini_machines[3],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_3,
                           &test_data.mini_machines[4],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_2,
                           &test_data.mini_machines[5],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_1,
                           &test_data.mini_machines[6],
                           sizeof(struct MiniGameStateMachine));
}

void test_register_all_prioritized_state_machine_small() {
  struct MiniGameStateMachine mock_registration_data_0 = {0};
  mock_registration_data_0.display_name = "TestSM0";
  mock_registration_data_0.priority = 0;
  mock_registration_data_0.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_1 = {0};
  mock_registration_data_1.display_name = "TestSM1";
  mock_registration_data_1.priority = 1;
  mock_registration_data_1.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_2 = {0};
  mock_registration_data_2.display_name = "TestSM2";
  mock_registration_data_2.priority = 2;
  mock_registration_data_2.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_3 = {0};
  mock_registration_data_3.display_name = "TestSM-2";
  mock_registration_data_3.priority = -2;
  mock_registration_data_3.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_4 = {0};
  mock_registration_data_4.display_name = "TestSM-1";
  mock_registration_data_4.priority = -1;
  mock_registration_data_4.next_state = mock_next_state;

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(1, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(2, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(3, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(4, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(5, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(6, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(7, test_data.mini_machines_offset);

  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_1,
                           &test_data.mini_machines[0],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_2,
                           &test_data.mini_machines[1],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[2],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[3],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[4],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_3,
                           &test_data.mini_machines[5],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_4,
                           &test_data.mini_machines[6],
                           sizeof(struct MiniGameStateMachine));
}

void test_register_all_prioritized_state_machine_big() {
  struct MiniGameStateMachine mock_registration_data_0 = {0};
  mock_registration_data_0.display_name = "TestSM0";
  mock_registration_data_0.priority = 0;
  mock_registration_data_0.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_1 = {0};
  mock_registration_data_1.display_name = "TestSM1";
  mock_registration_data_1.priority = 1;
  mock_registration_data_1.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_2 = {0};
  mock_registration_data_2.display_name = "TestSM2";
  mock_registration_data_2.priority = 2;
  mock_registration_data_2.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_3 = {0};
  mock_registration_data_3.display_name = "TestSM3";
  mock_registration_data_3.priority = 3;
  mock_registration_data_3.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_4 = {0};
  mock_registration_data_4.display_name = "TestSM4";
  mock_registration_data_4.priority = 4;
  mock_registration_data_4.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_5 = {0};
  mock_registration_data_5.display_name = "TestSM-4";
  mock_registration_data_5.priority = -4;
  mock_registration_data_5.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_6 = {0};
  mock_registration_data_6.display_name = "TestSM-3";
  mock_registration_data_6.priority = -3;
  mock_registration_data_6.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_7 = {0};
  mock_registration_data_7.display_name = "TestSM-2";
  mock_registration_data_7.priority = -2;
  mock_registration_data_7.next_state = mock_next_state;

  struct MiniGameStateMachine mock_registration_data_8 = {0};
  mock_registration_data_8.display_name = "TestSM-1";
  mock_registration_data_8.priority = -1;
  mock_registration_data_8.next_state = mock_next_state;

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_4);
  TEST_ASSERT_EQUAL_INT(1, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(2, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_8);
  TEST_ASSERT_EQUAL_INT(3, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(4, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_3);
  TEST_ASSERT_EQUAL_INT(5, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(6, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_7);
  TEST_ASSERT_EQUAL_INT(7, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(8, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_2);
  TEST_ASSERT_EQUAL_INT(9, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(10, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_6);
  TEST_ASSERT_EQUAL_INT(11, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(12, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_1);
  TEST_ASSERT_EQUAL_INT(13, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(14, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_5);
  TEST_ASSERT_EQUAL_INT(15, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(16, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(17, test_data.mini_machines_offset);

  gsm_sub_ops->add_mini_state_machine(mock_registration_data_0);
  TEST_ASSERT_EQUAL_INT(18, test_data.mini_machines_offset);

  /* for (size_t i = 0; i < test_data.mini_machines_offset; i++) { */
  /*   logging_ops_->log_info("test", */
  /*                              (char
   * *)test_data.mini_machines[i]->id); */
  /* } */

  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_1,
                           &test_data.mini_machines[0],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_2,
                           &test_data.mini_machines[1],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_3,
                           &test_data.mini_machines[2],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_4,
                           &test_data.mini_machines[3],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[4],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[5],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[6],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[7],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[8],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[9],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[10],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[11],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[12],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_0,
                           &test_data.mini_machines[13],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_5,
                           &test_data.mini_machines[14],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_6,
                           &test_data.mini_machines[15],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_7,
                           &test_data.mini_machines[16],
                           sizeof(struct MiniGameStateMachine));
  TEST_ASSERT_EQUAL_MEMORY(&mock_registration_data_8,
                           &test_data.mini_machines[17],
                           sizeof(struct MiniGameStateMachine));
}

int mock_next_state(struct GameStateMachineInput input,
                    struct GameStateMachineState *state) {
  return mock_state;
}

struct GameSmSubsystem *mock_get_subsystem(void) { return &test_data; };
