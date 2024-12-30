#ifndef USER_MOVE_H
#define USER_MOVE_H
/*******************************************************************************
 * @file user_move.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stddef.h>

#include "game/game.h"
#include "game/game_state_machine/game_states.h"
#include "input/input.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

extern struct InitRegistrationData init_user_move_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmUserMoveModuleOps {
  int (*init)(void);
};

struct GameSmUserMoveModuleOps *get_game_sm_user_move_module_ops(void);

#endif // USER_MOVE_H