#include "game/game_state_machine/game_sm_subsystem.h"

struct GameSmQuitModulePrivateOps {
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
};

struct GameSmQuitModulePrivateOps *get_game_sm_quit_module_priv_ops(void);
