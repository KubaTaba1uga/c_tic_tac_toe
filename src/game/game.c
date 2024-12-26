/*******************************************************************************
 * @file game.c
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// App's internal libs
#include "config/config.h"
/* #include "display/display.h" */
#include "game/game.h"
#include "game/game_state_machine/game_state_machine.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct ConfigRegistrationData *user1_input, *user2_input;

/* static int game_init(void); */
static int game_process_user1(enum InputEvents input_event);
static int game_process_user2(enum InputEvents input_event);
static int game_process(enum InputEvents input_event);
/* static int game_process_and_display(enum InputEvents input_event, */
/* enum Users input_user); */

/* static char module_id[] = "game"; */

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GamePrivateOps {
  int (*user1_logic)(enum InputEvents input_event);
  int (*user2_logic)(enum InputEvents input_event);
  int (*logic)(enum InputEvents input_event);
  int (*logic_and_display)(enum InputEvents input_event, enum Users input_user);
};

static struct GamePrivateOps game_priv_ops = {
    .user1_logic = game_process_user1,
    .user2_logic = game_process_user2,
    .logic = game_process,
    /* .logic_and_display = */
    /* game_process_and_display */

};

static struct GameOps game_ops = {.private_ops = &game_priv_ops};
struct GameOps *get_game_ops(void) { return &game_ops; }

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
/* struct InitRegistrationData init_game_reg = { */
/*     .id = module_id, */
/*     .init = game_init, */
/*     .destroy = NULL, */
/* }; */

/*******************************************************************************
 *    API
 ******************************************************************************/
int game_process_user1(enum InputEvents input_event) {
  return game_priv_ops.logic_and_display(input_event, User1);
}

int game_process_user2(enum InputEvents input_event) {
  return game_priv_ops.logic_and_display(input_event, User2);
}

int game_process(enum InputEvents input_event) {
  return game_priv_ops.logic_and_display(input_event, UserNone);
}

/* int game_process_and_display(enum InputEvents input_event, */
/*                              enum Users input_user) { */
/*   struct GameStateMachineState *game_state_machine; */
/*   struct LoggingUtilsOps *logging_ops; */
/*   struct DataToDisplay display_data; */
/*   struct DisplayOps *display_ops; */

/*   int err; */

/*   logging_ops = get_logging_utils_ops(); */
/*   display_ops = get_display_ops(); */

/*   err = game_sm_ops.step(input_event, input_user); */
/*   if (err) { */
/*     logging_ops->log_err(module_id, "Unable to perform step for user %i.", */
/*                          input_user); */

/*     return err; */
/*   } */

/*   game_state_machine = game_sm_ops.get_state_machine(); */
/*   if (!game_state_machine) { */
/*     logging_ops->log_err(module_id, "No state machine."); */

/*     return EINVAL; */
/*   } */

/*   display_data.user = game_state_machine->current_user; */
/*   display_data.state = game_state_machine->current_state; */
/*   display_data.moves_counter = game_state_machine->users_moves_count; */
/*   memcpy(display_data.moves, game_state_machine->users_moves_data, */
/*          sizeof(game_state_machine->users_moves_data)); */

/*   err = display_ops->display(&display_data); */
/*   if (err) { */
/*     logging_ops->log_err(module_id, "Unable to display game for user %i.", */
/*                          input_user); */
/*     return err; */
/*   } */

/*   return 0; */
/* } */

/* int game_init(void) { */
/*   /\* struct LoggingUtilsOps *logging_ops; *\/ */
/*   /\* struct ConfigOps *config_ops; *\/ */
/*   /\* struct InputOps *input_ops; *\/ */
/*   /\* int err; *\/ */

/*   /\* logging_ops = get_logging_utils_ops(); *\/ */
/*   /\* config_ops = get_config_ops(); *\/ */
/*   /\* input_ops = get_input_ops(); *\/ */

/*   return 0; */
/* } */
