#ifndef DISPLAY_SM_MODULE_H
#define DISPLAY_SM_MODULE_H

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmDisplayModuleOps {
  int (*init)(void);
};

struct GameSmDisplayModuleOps *get_game_sm_display_module_ops(void);

#endif // DISPLAY_SM_MODULE_H
