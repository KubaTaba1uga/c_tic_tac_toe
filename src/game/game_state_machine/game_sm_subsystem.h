#ifndef GAME_SM_SUBSYSTEM_H
#define GAME_SM_SUBSYSTEM_H
/*******************************************************************************
 * @file game_sm_subsystem.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
typedef int (*gsm_subsystem_next_state_function_t)(
    struct GameStateMachineInput input, struct GameStateMachineState *state);

struct GameSmSubsystemRegistrationData {
  const char *id;
  gsm_subsystem_next_state_function_t next_state;
  int priority;
};

typedef void (*gsm_subsystem_register_state_machine_function_t)(
    struct GameSmSubsystemRegistrationData *registration_data);

struct GameSmSubsystemOps {
  gsm_subsystem_next_state_function_t next_state;
  gsm_subsystem_register_state_machine_function_t register_state_machine;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct GameSmSubsystemOps game_sm_subsystem_ops;

#endif // GAME_SM_SUBSYSTEM_H
