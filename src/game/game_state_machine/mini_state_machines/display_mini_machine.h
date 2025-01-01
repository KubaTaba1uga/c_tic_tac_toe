#ifndef DISPLAY_SM_MODULE_H
#define DISPLAY_SM_MODULE_H

#include "game/game_state_machine/game_sm_subsystem.h"

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmDisplayModuleOps {
  int (*init)(void);
};

struct GameSmDisplayModuleOps *get_game_sm_display_module_ops(void);

#endif // DISPLAY_SM_MODULE_H
