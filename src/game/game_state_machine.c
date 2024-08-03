/*******************************************************************************
 * @file game_state_machine.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library

// App's internal libs
#include "game/game.h"
#include "input/input.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
enum GameStates {
  GameStateUser1Turn,
  GameStateUser2Turn,
};

struct GameStateMachine {
  enum GameStates state;
};

struct GameStateMachine game_state_machine;
/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int process_game_state_machine(enum InputEvents input_event) {
  if (input_event <= INPUT_EVENT_NONE || input_event >= INPUT_EVENT_MAX)
    return 0;

  return 0;
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
