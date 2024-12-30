#ifndef MOVESCLEANUP_SM_MODULE_H
#define MOVESCLEANUP_SM_MODULE_H
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

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
extern struct InitRegistrationData init_clean_last_move_state_machine_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmCleanLastMoveModuleOps {
  int (*init)(void);
};

struct GameSmCleanLastMoveModuleOps *get_game_sm_movescleanup_module_ops(void);

#endif // MOVESCLEANUP_SM_MODULE_H