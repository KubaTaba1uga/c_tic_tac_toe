#ifndef WIN_SM_MODULE_H
#define WIN_SM_MODULE_H

struct GameSmWinModuleOps {
  int (*init)(void);
};

struct GameSmWinModuleOps *get_game_sm_win_module_ops(void);

#endif // WIN_SM_MODULE_H
