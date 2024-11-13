#include "game/game_state_machine/game_sm_subsystem.h"

struct GameSmQuitModulePrivateOps {
  int (*init)(void);
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
};
