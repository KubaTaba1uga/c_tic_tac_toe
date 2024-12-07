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
struct GameSmSubsystemRegistrationData {
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
  const char *id;
  int priority;
};

extern struct InitRegistrationData init_game_sm_sub_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmSubsystemOps {
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
  void (*register_state_machine)(
      struct GameSmSubsystemRegistrationData *registration_data);
  void *private_ops;
};

struct GameSmSubsystemOps *get_game_sm_subsystem_ops(void);

#endif // GAME_SM_SUBSYSTEM_H
