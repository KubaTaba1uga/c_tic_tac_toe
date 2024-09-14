#ifndef GAME_STATES_H
#define GAME_STATES_H
/*******************************************************************************
 * @file game_states.h
 * @brief This file contains all states handled by GSM.
 *
 * When adding new feature to game state machine, one usually implements new sub
 *  state machine. The main con of this deisgn is that GameStates enum needs to
 *  hold all possible states for all sub game state machines. Otherwise display
 *  couldn't show customized screen if sub gsm would require it.
 *
 ******************************************************************************/

#include "game/game.h"

enum GameStates {
  GameStateUser1 = User1,
  GameStateUser2 = User2,
  GameStateQuitting,
  GameStateQuit,
  GameStateWin,
};

#endif
