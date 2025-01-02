/*******************************************************************************
 * @file quit_sm_module.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// App's internal libs
#include "game/game.h"
#include "game/game_config.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/mini_state_machines/common.h"
#include "game/game_state_machine/mini_state_machines/win_mini_machine.h"
#include "game/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
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
static char gsm_win_module_id[] = "win_sm_module";
static struct GameStateMachineCommonOps *gsm_common_ops;
static struct GameConfigOps *game_config_ops;
static struct GameOps *game_ops;
static struct GameSmWinModulePrivateOps *win_priv_ops;
struct GameSmWinModulePrivateOps *get_game_sm_win_module_priv_ops(void);

/*******************************************************************************
 *    API
 ******************************************************************************/
static int win_state_machine_init(void) {
  struct GameSmSubsystemOps *gsm_sub_ops = get_game_sm_subsystem_ops();

  game_ops = get_game_ops();
  gsm_common_ops = get_sm_mini_machines_common_ops();
  game_config_ops = get_game_config_ops();
  win_priv_ops = get_game_sm_win_module_priv_ops();

  struct MiniGameStateMachine win_mini_machine = {
      .next_state = win_priv_ops->next_state,
      .display_name = gsm_win_module_id,
      .priority = 4};

  gsm_sub_ops->add_mini_state_machine(win_mini_machine);

  return 0;
}

static int win_state_machine_next_state(struct GameStateMachineInput input,
                                        struct GameStateMachineState *state) {
  struct UserMove *current_user_move = gsm_common_ops->get_last_move(state);
  int users_amount;
  bool is_win;
  int err;

  if (state->current_state == GameStateWinning) {
    state->current_state = GameStateWin;
    game_ops->stop();
    return 0;
  }

  if (!current_user_move) {
    return 0;
  }

  if (current_user_move->type != USER_MOVE_TYPE_SELECT_VALID) {
    return 0;
  }

  err = game_config_ops->get_users_amount(&users_amount);
  if (err) {
    return err;
  }

  is_win = win_priv_ops->process_vertical_win(current_user_move,
                                              state->users_moves_offset - 1,
                                              state->users_moves, users_amount);
  if (!is_win) {
    is_win = win_priv_ops->process_horizontal_win(
        current_user_move, state->users_moves_offset - 1, state->users_moves,
        users_amount);
  }
  if (!is_win) {
    is_win = win_priv_ops->process_diagonal_win(
        current_user_move, state->users_moves_offset - 1, state->users_moves,
        users_amount);
  }

  if (is_win) {
    state->current_state = GameStateWinning;
    return 0;
  }

  return 0;
};

static bool
win_state_machine_process_vertical_win(struct UserMove *current_user_move,
                                       size_t n, struct UserMove users_moves[n],
                                       size_t users_amount) {
  struct UserMove *tmp_user_move;
  size_t win_moves_counter = 0;

  for (size_t i = 0; i < n; i++) {
    tmp_user_move = &users_moves[i];

    if (tmp_user_move->user_id == current_user_move->user_id &&
        tmp_user_move->coordinates.x == current_user_move->coordinates.x) {
      win_moves_counter++;
    }
  }

  return win_moves_counter >= users_amount;
}

static bool
win_state_machine_process_horizontal_win(struct UserMove *current_user_move,
                                         size_t n, struct UserMove *users_moves,
                                         size_t users_amount) {
  struct UserMove *tmp_user_move;
  size_t win_moves_counter = 0;

  for (size_t i = 0; i < n; i++) {
    tmp_user_move = &users_moves[i];

    if (tmp_user_move->user_id == current_user_move->user_id &&
        tmp_user_move->coordinates.y == current_user_move->coordinates.y) {
      win_moves_counter++;
    }
  }

  return win_moves_counter >= users_amount;
}

static bool
win_state_machine_process_diagonal_win(struct UserMove *current_user_move,
                                       size_t n, struct UserMove *users_moves,
                                       size_t users_amount) {
  bool is_win;

  is_win = win_priv_ops->process_diagonal_win_a(current_user_move, n,
                                                users_moves, users_amount);

  return is_win;
}

static bool
win_state_machine_process_diagonal_win_a(struct UserMove *current_user_move,
                                         size_t n, struct UserMove *users_moves,
                                         size_t users_amount) {
  struct UserMove *tmp_user_move;
  size_t win_moves_counter = 0;

  for (size_t xy_i = current_user_move->coordinates.x + 1; xy_i <= users_amount;
       xy_i++) {
    for (size_t i = 0; i < n; i++) {
      tmp_user_move = &users_moves[i];
      if (tmp_user_move->user_id == current_user_move->user_id) {
        if ((tmp_user_move->coordinates.y ==
                 current_user_move->coordinates.y - xy_i &&
             tmp_user_move->coordinates.x ==
                 current_user_move->coordinates.x + xy_i) ||
            (tmp_user_move->coordinates.y ==
                 current_user_move->coordinates.y + xy_i &&
             tmp_user_move->coordinates.x ==
                 current_user_move->coordinates.x - xy_i)) {
          win_moves_counter++;
        }
      }
    }
  }

  return win_moves_counter >= users_amount;
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/

static struct GameSmWinModulePrivateOps private_ops = {
    .next_state = win_state_machine_next_state,
    .process_horizontal_win = win_state_machine_process_horizontal_win,
    .process_vertical_win = win_state_machine_process_vertical_win,
    .process_diagonal_win = win_state_machine_process_diagonal_win,
    .process_diagonal_win_a = win_state_machine_process_diagonal_win_a,
};

static struct GameSmWinModuleOps game_sm_win_ops = {.init =
                                                        win_state_machine_init};

struct GameSmWinModuleOps *get_game_sm_win_module_ops(void) {
  return &game_sm_win_ops;
}

struct GameSmWinModulePrivateOps *get_game_sm_win_module_priv_ops(void) {
  return &private_ops;
};
