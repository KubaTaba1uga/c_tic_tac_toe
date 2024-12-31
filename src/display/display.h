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
#include "game/game_user.h"
#include "game/user_move.h"
#include "static_array_lib.h"
#include <stddef.h>

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define DISPLAY_CLI_NAME "cli"

struct DisplayData {
  enum GameStates game_state;
  int display_id;
  int user_id;
  // User moves are passed as reference from game state machine,
  //  we are not copying from performence reasons but we still
  //  want to prevent display from changing users moves.
  const struct UserMove *moves;
  size_t moves_length;
};

typedef int (*display_display_func_t)(struct DisplayData *data);

struct DisplayDisplay {
  display_display_func_t display;
  const char *display_name;
};

struct DisplayOps {
  int (*init)(void); // We will add display config var in game_config.c
  //  Each display registers itself to display on init and game_config
  //  just chooses which one will be used during game session.
  display_display_func_t display;
  int (*add_display)(struct DisplayDisplay *new_display);
  int (*get_display_id)(const char *display_name, int **id_placeholder);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct DisplayOps *get_display_ops(void);

#endif // DISPLAY_H
