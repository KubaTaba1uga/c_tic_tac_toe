/*******************************************************************************
 * @file game_sm_subsystem.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <stddef.h>

// App's internal libs
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS 100

struct GameSmSubsystem {
  size_t count;
  struct GameSmSubsystemRegistrationData
      *registrations[MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS];
};

static void register_state_machine(
    struct GameSmSubsystemRegistrationData *registration_data);
static enum GameStates get_next_state(struct GameSmNextStateCreationData data);

static char module_id[] = "game_logic_sm";
static struct GameSmSubsystem game_sm_subsystem = {.count = 0};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct GameSmSubsystemOps game_sm_subsystem_ops = {.next_state = get_next_state,
                                                   .register_state_machine =
                                                       register_state_machine};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
void register_state_machine(
    struct GameSmSubsystemRegistrationData *registration_data) {
  if (game_sm_subsystem.count < MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS) {
    game_sm_subsystem.registrations[game_sm_subsystem.count++] =
        registration_data;
  } else {
    logging_utils_ops.log_err(module_id,
                              "Unable to register %s in game logic SM, "
                              "no enough space in `registrations` array.",
                              registration_data->id);
  }
}

enum GameStates get_next_state(struct GameSmNextStateCreationData data) {
  enum GameStates new_game_state;
  size_t i;

  for (i = 0; i < game_sm_subsystem.count; i++) {
    logging_utils_ops.log_info(module_id, "Processing %s",
                               game_sm_subsystem.registrations[i]->id);

    new_game_state = game_sm_subsystem.registrations[i]->callback(data);

    if (data.current_state != new_game_state)
      return new_game_state;
  }

  return data.current_state;
};
