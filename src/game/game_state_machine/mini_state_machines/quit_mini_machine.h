#ifndef QUIT_SM_MODULE_H
#define QUIT_SM_MODULE_H
/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmQuitModuleOps {
  int (*init)(void);
};

struct GameSmQuitModuleOps *get_game_sm_quit_module_ops(void);

#endif // QUIT_SM_MODULE_H
