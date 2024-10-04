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
#include <string.h>

// App's internal libs
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "init/init.h"
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

static int game_sm_subsystem_init(void);
static void register_state_machine(
    struct GameSmSubsystemRegistrationData *registration_data);
static int get_next_state(struct GameStateMachineInput input,
                          struct GameStateMachineState *state);
static void priority_handle_new_registration(void);
static void
priority_handle_positive_value(struct GameSmSubsystemRegistrationData *new_reg);
static void
priority_handle_negative_value(struct GameSmSubsystemRegistrationData *new_reg);
static void
priority_handle_no_value(struct GameSmSubsystemRegistrationData *new_reg);
static void
insert_registration(int start, struct GameSmSubsystemRegistrationData *new_reg);

static struct GameSmSubsystem game_sm_subsystem = {.count = 0};
static struct LoggingUtilsOps *logging_ops;
static char module_id[] = "game_logic_sm";

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_game_sm_sub_reg = {
    .id = module_id,
    .init_func = game_sm_subsystem_init,
    .destroy_func = NULL,
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct GameSmSubsystemOps game_sm_subsystem_ops = {.next_state = get_next_state,
                                                   .register_state_machine =
                                                       register_state_machine};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int game_sm_subsystem_init(void) {
  logging_ops = get_logging_utils_ops();
  return 0;
}

void register_state_machine(
    struct GameSmSubsystemRegistrationData *registration_data) {
  if (game_sm_subsystem.count < MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS) {
    game_sm_subsystem.registrations[game_sm_subsystem.count++] =
        registration_data;
  } else {
    logging_ops->log_err(module_id,
                         "Unable to register %s in game logic SM, "
                         "no enough space in `registrations` array.",
                         registration_data->id);
    return;
  }

  priority_handle_new_registration();
}

int get_next_state(struct GameStateMachineInput input,
                   struct GameStateMachineState *data) {
  size_t i;
  int err;

  for (i = 0; i < game_sm_subsystem.count; i++) {
    logging_ops->log_info(module_id, "Processing %s",
                          game_sm_subsystem.registrations[i]->id);

    err = game_sm_subsystem.registrations[i]->next_state(input, data);

    if (err) {
      logging_ops->log_err(module_id, "Unable to process %s: %s",
                           game_sm_subsystem.registrations[i]->id,
                           strerror(err));
      return err;
    }
  }

  return 0;
};

void priority_handle_new_registration(void) {
  struct GameSmSubsystemRegistrationData *new_reg =
      game_sm_subsystem.registrations[game_sm_subsystem.count - 1];

  if (new_reg->priority > 0)
    priority_handle_positive_value(new_reg);
  else if (new_reg->priority < 0)
    priority_handle_negative_value(new_reg);
  else // if (new_reg->priority == 0)
    priority_handle_no_value(new_reg);
}

void priority_handle_positive_value(
    struct GameSmSubsystemRegistrationData *new_reg) {
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t i;

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

void priority_handle_negative_value(
    struct GameSmSubsystemRegistrationData *new_reg) {
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t i = game_sm_subsystem.count;

  while (i-- > 0) {
    tmp_reg = game_sm_subsystem.registrations[i];

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
}

void priority_handle_no_value(struct GameSmSubsystemRegistrationData *new_reg) {
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t i;

  for (i = 0; i < game_sm_subsystem.count; i++) {
    tmp_reg = game_sm_subsystem.registrations[i];

    if (new_reg->priority > tmp_reg->priority) {
      insert_registration(i, new_reg);
      break;
    }
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

INIT_REGISTER_SUBSYSTEM_CHILD(&init_game_sm_sub_reg, init_game_reg_p);
