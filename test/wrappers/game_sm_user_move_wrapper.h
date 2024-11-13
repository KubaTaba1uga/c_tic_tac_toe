#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"

struct UserMoveCoordinates {
  int width;
  int height;
};

struct UserMoveStateMachineState {
  struct UserMoveCoordinates coordinates;
};

struct GameSmUserMoveModulePrivateOps {
  int (*init)(void);
  struct UserMoveStateMachineState *(*get_state)(void);
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
  void (*set_default_state)(void);
  void (*handle_up_event)(struct UserMoveCoordinates *coordinates,
                          struct UserMove *new_user_move);
  void (*handle_down_event)(struct UserMoveCoordinates *coordinates,
                            struct UserMove *new_user_move);
  void (*handle_left_event)(struct UserMoveCoordinates *coordinates,
                            struct UserMove *new_user_move);
  void (*handle_right_event)(struct UserMoveCoordinates *coordinates,
                             struct UserMove *new_user_move);
  void (*handle_exit_event)(struct UserMoveCoordinates *coordinates,
                            struct UserMove *new_user_move);
  void (*handle_select_event)(struct UserMoveCoordinates *coordinates,
                              struct UserMove *new_user_move,
                              struct GameStateMachineState *data);
};
