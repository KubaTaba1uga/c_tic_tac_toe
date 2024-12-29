#include "static_array_lib.h"

#include "game/game_state_machine/game_state_machine.h"

#define GAME_SM_MINI_MACHINES_MAX 100

struct GameSmSubsystem {
  SARRS_FIELD(mini_machines, struct MiniGameStateMachine,
              GAME_SM_MINI_MACHINES_MAX);
};

typedef struct GameSmSubsystem GameSmSubsystem;

SARRS_DECL(GameSmSubsystem, mini_machines, struct MiniGameStateMachine,
           GAME_SM_MINI_MACHINES_MAX);

struct GameSmSubsystemPrivateOps {
  int (*init)(void);
  void (*priority_handle_new_registration)(void);
  void (*priority_handle_positive_value)(struct MiniGameStateMachine *new_reg);
  void (*priority_handle_negative_value)(struct MiniGameStateMachine *new_reg);
  void (*priority_handle_no_value)(struct MiniGameStateMachine *new_reg);
  void (*insert_registration)(int start, struct MiniGameStateMachine *new_reg);
  struct GameSmSubsystem *(*get_subsystem)(void);
};

struct GameSmSubsystemPrivateOps *get_gsm_sub_private_ops(void);
