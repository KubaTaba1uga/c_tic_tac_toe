#ifndef GAME_SM_SUBSYSTEM_H
#define GAME_SM_SUBSYSTEM_H

#include "game/game_state_machine/game_state_machine.h"

struct MiniGameStateMachine {
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
  const char *display_name;
  int priority;
};

struct GameSmSubsystemOps {
  int (*init)(void);
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
  int (*add_mini_state_machine)(struct MiniGameStateMachine mini_state_machine);
  int (*display_starting_screen)(void);
};

struct GameSmSubsystemOps *get_game_sm_subsystem_ops(void);

#endif // GAME_SM_SUBSYSTEM_H
