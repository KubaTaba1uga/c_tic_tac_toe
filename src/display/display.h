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

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define DISPLAY_MAX_USERS_MOVES 100

struct DataToDisplay {
  enum Users current_user;
  enum GameStates game_state;
  struct UserMove *moves;
};

typedef int (*display_display_func_t)(void);

struct DisplayRegistrationData {
  display_display_func_t display;
  const char *id;
};

struct DisplayOps {
  int (*display)(struct DataToDisplay *data);
  void (*register_module)(
      struct InputRegistrationData *input_registration_data);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct DisplayOps *get_display_ops(void);

#endif // DISPLAY_H
