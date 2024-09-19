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
static void
insert_registration(int start, struct GameSmSubsystemRegistrationData *new_reg);

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

  // Do not handle negative/no priority
  if (new_reg->priority <= 0)
    return;

  for (i = 0; i < game_sm_subsystem.count; i++) {
    tmp_reg = game_sm_subsystem.registrations[i];

    // If positive priority 1 is biggest value. The bigger
    //  number the smaller priority.
    if (
        // If comparing vs negative/no priority, write before.
        (tmp_reg->priority <= 0) ||
        // If comparing to positive priority with smaller number,
        //  write before tmp_reg.
        (new_reg->priority < tmp_reg->priority)) {
      insert_registration(i, new_reg);
      break;
    }
  }
}

void handle_negative_priority(void) {
  struct GameSmSubsystemRegistrationData *new_reg =
      game_sm_subsystem.registrations[game_sm_subsystem.count - 1];
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t i = game_sm_subsystem.count;

  // Do not handle positive/no priority
  if (new_reg->priority >= 0)
    return;

  while (i-- > 0) {
    tmp_reg = game_sm_subsystem.registrations[i];
    logging_utils_ops.log_info("test", "Compare %s to %s", new_reg->id,
                               tmp_reg->id);

    // If negative priority -1 is smallest value. The smaller
    //  number the bigger priority.
    if (
        // If comparing vs positive/no priority, write after.
        (tmp_reg->priority >= 0) ||
        // If comparing to negative priority with bigger number,
        //  write after tmp_reg.
        (new_reg->priority > tmp_reg->priority)) {
      insert_registration(i + 1, new_reg);
      break;
    }
  }

  for (size_t i = 0; i < game_sm_subsystem.count; i++) {
    logging_utils_ops.log_info("test",
                               (char *)game_sm_subsystem.registrations[i]->id);
  }
}

void insert_registration(int start,
                         struct GameSmSubsystemRegistrationData *new_reg) {
  size_t i = game_sm_subsystem.count;

  // Move registrations by 1 right.
  while (i-- > start) {
    game_sm_subsystem.registrations[i + 1] = game_sm_subsystem.registrations[i];
  }

  game_sm_subsystem.registrations[start] = new_reg;
}
