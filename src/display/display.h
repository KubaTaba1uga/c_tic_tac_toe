#ifndef DISPLAY_H
#define DISPLAY_H
/*******************************************************************************
 * @file display.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include "game/game.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include <stddef.h>

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

#define DISPLAY_CLI_NAME "cli"
#define DISPLAY_MAX_USERS_MOVES 100

struct DataToDisplay {
  enum Users user;
  enum GameStates state;
  struct UserMove moves[DISPLAY_MAX_USERS_MOVES];
  size_t moves_counter;
};

typedef int (*display_display_func_t)(struct DataToDisplay *data);

struct DisplayRegistrationData {
  display_display_func_t display;
  const char *id;
};

struct DisplayOps {
  display_display_func_t display;
  void (*register_module)(struct DisplayRegistrationData *registration_data);
  void *private_ops;
};

extern struct InitRegistrationData *init_display_reg_p;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct DisplayOps *get_display_ops(void);

#endif // DISPLAY_H
