#ifndef USER_TURN_SM_MODULE_H
#define USER_TURN_SM_MODULE_H

struct GameSmUserTurnModuleOps {
  int (*init)(void);
};

struct GameSmUserTurnModuleOps *get_game_sm_user_turn_module_ops(void);

#endif // USER_TURN_SM_MODULE_H
