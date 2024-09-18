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
struct GameStateMachineState get_next_state(struct GameStateMachineInput input,
                                            struct GameStateMachineState state);
static void handle_positive_priority();
static void handle_negative_priority();

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
    return;
  }

  handle_positive_priority();
  handle_negative_priority();
}

struct GameStateMachineState get_next_state(struct GameStateMachineInput input,
                                            struct GameStateMachineState data) {
  struct GameStateMachineState new_data;
  size_t i;

  for (i = 0; i < game_sm_subsystem.count; i++) {
    logging_utils_ops.log_info(module_id, "Processing %s",
                               game_sm_subsystem.registrations[i]->id);

    new_data = game_sm_subsystem.registrations[i]->next_state(input, data);

    if (data.current_state != new_data.current_state)
      return new_data;
  }

  return data;
};

void handle_positive_priority(void) {
  struct GameSmSubsystemRegistrationData *new_reg =
      game_sm_subsystem.registrations[game_sm_subsystem.count - 1];
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t i;

  if (new_reg->priority <= 0)
    return;

  logging_utils_ops.log_info(module_id, "Start for %s", new_reg->id);

  for (i = 0; i < game_sm_subsystem.count; i++) {
    tmp_reg = game_sm_subsystem.registrations[i];
    // Do not handle negative priority.
    if (tmp_reg->priority < 0)
      continue;

    if ( // If no priority, write new_reg before tmp_reg.
        (tmp_reg->priority == 0) ||
        // 1 is biggest priority. The bigger number the smaller priority.
        (new_reg->priority <= tmp_reg->priority)) {
      game_sm_subsystem.registrations[i] = new_reg;
      game_sm_subsystem.registrations[game_sm_subsystem.count - 1] = tmp_reg;
      break;
    }
  }
}

void handle_negative_priority(void) {
  struct GameSmSubsystemRegistrationData *new_reg =
      game_sm_subsystem.registrations[game_sm_subsystem.count - 1];
  struct GameSmSubsystemRegistrationData *tmp_reg, *nested_tmp_reg;
  size_t i, k;

  if (new_reg->priority >= 0)
    return;

  for (i = 0; i < game_sm_subsystem.count; i++) {
    tmp_reg = game_sm_subsystem.registrations[i];

    // If positive/no priority skip. Negative priorities are at the end.
    if (tmp_reg->priority >= 0)
      continue;

    // -1 is smallest priority. The smaller number the smaller priority.
    // If given -2 and -5, -2 should be after -5.
    if (new_reg->priority >= tmp_reg->priority) {
      game_sm_subsystem.registrations[i] = new_reg;

      for (k = i + 1; k < game_sm_subsystem.count; k++) {
        nested_tmp_reg = game_sm_subsystem.registrations[k];
        game_sm_subsystem.registrations[k] = tmp_reg;
        tmp_reg = nested_tmp_reg;
      }

      break;
    }
  }
}
