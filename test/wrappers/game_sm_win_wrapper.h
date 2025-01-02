#include <stdbool.h>

#include "game/game_state_machine/game_state_machine.h"
#include "game/user_move.h"

struct GameSmWinModulePrivateOps {
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
  bool (*process_vertical_win)(struct UserMove *current_user_move, size_t n,
                               struct UserMove users_moves[n],
                               size_t users_amount);
  bool (*process_horizontal_win)(struct UserMove *current_user_move, size_t n,
                                 struct UserMove users_moves[n],
                                 size_t users_amount);
  bool (*process_diagonal_win)(struct UserMove *current_user_move, size_t n,
                               struct UserMove users_moves[n],
                               size_t users_amount);
  bool (*process_diagonal_win_a)(struct UserMove *current_user_move, size_t n,
                                 struct UserMove users_moves[n],
                                 size_t users_amount);
  bool (*process_diagonal_win_b)(struct UserMove *current_user_move, size_t n,
                                 struct UserMove users_moves[n],
                                 size_t users_amount);
};

struct GameSmWinModulePrivateOps *get_game_sm_win_module_priv_ops(void);
