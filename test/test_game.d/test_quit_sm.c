/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// Tests framework
#include <unity.h>

// App's internal libs
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/sub_state_machines/quit_sm_module.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

// Mocks requirement
#include "game_sm_quit_wrapper.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static int destroy_counter;
static void mock_input_ops_destroy();
static struct LoggingUtilsOps *logging_ops;
static struct GameSmQuitModulePrivateOps *quit_sm_ops;
static struct InputOps *input_ops;
static struct InitOps *init_ops;

/*******************************************************************************
 *    TESTS FRAMEWORK BOILERCODE
 ******************************************************************************/
void setUp() {
  quit_sm_ops = get_game_sm_quit_module_ops()->private_ops;
  logging_ops = get_logging_utils_ops();
  input_ops = get_input_ops();
  init_ops = get_init_ops();

  // Disable game init and destroy
  init_game_reg.init = NULL;
  init_game_reg.destroy = NULL;
  init_ops->initialize_system();

  input_ops->destroy = mock_input_ops_destroy;

  destroy_counter = 0;
}

void tearDown() { init_ops->destroy_system(); }

/*******************************************************************************
 *    TESTS
 ******************************************************************************/
void test_quit_sm_get_quitting_state() {
  struct UserMove current_move = {
      .user = User1, .coordinates = {0, 0}, .type = USER_MOVE_TYPE_QUIT};
  struct GameStateMachineInput input = {.input_user = User1,
                                        .input_event = INPUT_EVENT_EXIT};
  struct GameStateMachineState state = {.current_state = GameStatePlay,
                                        .current_user = User1,
                                        .users_moves_count = 1,
                                        .users_moves_data = {current_move}};
  int err;

  err = quit_sm_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStateQuitting, state.current_state);
  TEST_ASSERT_EQUAL_INT(0, destroy_counter);
}

void test_quit_sm_get_quit_state() {
  struct UserMove current_move = {
      .user = User1, .coordinates = {0, 0}, .type = USER_MOVE_TYPE_QUIT};
  struct GameStateMachineInput input = {.input_user = User1,
                                        .input_event = INPUT_EVENT_EXIT};
  struct GameStateMachineState state = {.current_state = GameStateQuitting,
                                        .current_user = User1,
                                        .users_moves_count = 1,
                                        .users_moves_data = {current_move}};
  int err;
  err = quit_sm_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStateQuit, state.current_state);
  TEST_ASSERT_EQUAL_INT(1, destroy_counter);
}

void test_quit_sm_get_play_state() {
  struct UserMove current_move = {.user = User1,
                                  .coordinates = {0, 0},
                                  .type = USER_MOVE_TYPE_SELECT_VALID};
  struct GameStateMachineInput input = {.input_user = User1,
                                        .input_event = INPUT_EVENT_SELECT};
  struct GameStateMachineState state = {.current_state = GameStateQuitting,
                                        .current_user = User1,
                                        .users_moves_count = 1,
                                        .users_moves_data = {current_move}};
  int err;
  err = quit_sm_ops->next_state(input, &state);

  TEST_ASSERT_EQUAL_INT(0, err);
  TEST_ASSERT_EQUAL_INT(GameStatePlay, state.current_state);
  TEST_ASSERT_EQUAL_INT(0, destroy_counter);
}

void mock_input_ops_destroy() { destroy_counter++; }
