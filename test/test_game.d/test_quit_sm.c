/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <unity.h>

#include "game/game.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/mini_state_machines/common.h"
#include "game/game_state_machine/mini_state_machines/quit_mini_machine.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

#include "game_sm_quit_wrapper.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static struct GameSmQuitModuleOps *quit_sm_ops;
static struct LoggingUtilsOps *logging_ops;
static struct GameOps *game_ops;
static struct GameStateMachineCommonOps *gsm_common_ops;
static struct GameSmQuitModulePrivateOps *quit_priv_ops;
/*******************************************************************************
 *    TESTS FRAMEWORK BOILERCODE
 ******************************************************************************/
void setUp(void) {
  struct InitOps *init_ops;

  init_ops = get_init_ops();
  logging_ops = get_logging_utils_ops();
  quit_sm_ops = get_game_sm_quit_module_ops();
  gsm_common_ops = get_sm_mini_machines_common_ops();
  game_ops = get_game_ops();
  quit_priv_ops = get_game_sm_quit_module_priv_ops();

  // Initialize the system
  init_ops->initialize();
}

void tearDown(void) {
  struct InitOps *init_ops;
  init_ops = get_init_ops();
  init_ops->destroy();
}

/*******************************************************************************
 *    TESTS
 ******************************************************************************/
void test_quit_sm_transition_to_quitting(void) {
  struct UserMove current_move = {
      .type = USER_MOVE_TYPE_QUIT, .user_id = 0, .coordinates = {0, 0}};
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_EXIT,
                                        .device_id = 0};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = 0};

  gsm_common_ops->add_move(&state, current_move);

  int err = quit_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStateQuitting, state.current_state);
}

void test_quit_sm_transition_to_quit(void) {
  struct UserMove current_move = {
      .type = USER_MOVE_TYPE_QUIT, .user_id = 0, .coordinates = {0, 0}};
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_EXIT,
                                        .device_id = 0};
  struct GameStateMachineState state = {.current_state = GameStateQuitting,
                                        .current_user = 0};

  gsm_common_ops->add_move(&state, current_move);

  int err = quit_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStateQuit, state.current_state);
}

void test_quit_sm_cancel_quit(void) {
  struct UserMove current_move = {
      .type = USER_MOVE_TYPE_SELECT_VALID, .user_id = 0, .coordinates = {0, 0}};
  struct GameStateMachineInput input = {.input_event = INPUT_EVENT_SELECT,
                                        .device_id = 0};
  struct GameStateMachineState state = {.current_state = GameStateQuitting,
                                        .current_user = 0};

  gsm_common_ops->add_move(&state, current_move);

  int err = quit_priv_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStatePlay, state.current_state);
}
