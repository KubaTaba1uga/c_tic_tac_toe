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

static char game_sm_subsystem_module_id[] = "game_sm_subsystem";
static struct GameSmSubsystem game_sm_subsystem = {.count = 0};
static struct LoggingUtilsOps *logging_ops;

/* -------------------------------------------------------------------------- */

static int game_sm_subsystem_init(void);
static void game_sm_subsystem_register_state_machine(
    struct GameSmSubsystemRegistrationData *registration_data);
static int
game_sm_subsystem_get_next_state(struct GameStateMachineInput input,
                                 struct GameStateMachineState *state);
static void game_sm_subsystem_priority_handle_new_registration(void);
static void game_sm_subsystem_priority_handle_positive_value(
    struct GameSmSubsystemRegistrationData *new_reg);
static void game_sm_subsystem_priority_handle_negative_value(
    struct GameSmSubsystemRegistrationData *new_reg);
static void game_sm_subsystem_priority_handle_no_value(
    struct GameSmSubsystemRegistrationData *new_reg);
static void game_sm_subsystem_insert_registration(
    int start, struct GameSmSubsystemRegistrationData *new_reg);
static size_t *game_sm_subsystem_get_counter(void);
static struct GameSmSubsystemRegistrationData **
game_sm_subsystem_get_registrations(void);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmSubsystemPrivateOps {
  void (*priority_handle_new_registration)(void);
  void (*priority_handle_positive_value)(
      struct GameSmSubsystemRegistrationData *new_reg);
  void (*priority_handle_negative_value)(
      struct GameSmSubsystemRegistrationData *new_reg);
  void (*priority_handle_no_value)(
      struct GameSmSubsystemRegistrationData *new_reg);
  void (*insert_registration)(int start,
                              struct GameSmSubsystemRegistrationData *new_reg);
  size_t *(*get_counter)(void);
  struct GameSmSubsystemRegistrationData **(*get_registrations)(void);
};

static struct GameSmSubsystemPrivateOps priv_ops = {
    .priority_handle_new_registration =
        game_sm_subsystem_priority_handle_new_registration,
    .priority_handle_positive_value =
        game_sm_subsystem_priority_handle_positive_value,
    .priority_handle_negative_value =
        game_sm_subsystem_priority_handle_negative_value,
    .priority_handle_no_value = game_sm_subsystem_priority_handle_no_value,
    .insert_registration = game_sm_subsystem_insert_registration,
    .get_counter = game_sm_subsystem_get_counter,
    .get_registrations = game_sm_subsystem_get_registrations};

struct GameSmSubsystemOps game_sm_subsystem_ops = {
    .next_state = game_sm_subsystem_get_next_state,
    .register_state_machine = game_sm_subsystem_register_state_machine,
    .private_ops = &priv_ops};

struct GameSmSubsystemOps *get_game_sm_subsystem_ops(void) {
  return &game_sm_subsystem_ops;
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_game_sm_sub_reg = {
    .id = game_sm_subsystem_module_id,
    .init_func = game_sm_subsystem_init,
    .destroy_func = NULL,
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
void game_sm_subsystem_register_state_machine(
    struct GameSmSubsystemRegistrationData *registration_data) {
  struct GameSmSubsystemRegistrationData **registrations =
      priv_ops.get_registrations();
  size_t *counter = priv_ops.get_counter();

  if (*counter < MAX_GAME_SM_SUBSYSTEM_REGISTRATIONS) {
    registrations[(*counter)++] = registration_data;
  } else {
    logging_ops->log_err(game_sm_subsystem_module_id,
                         "Unable to register %s in game logic SM, "
                         "no enough space in `registrations` array.",
                         registration_data->id);
    return;
  }

  priv_ops.priority_handle_new_registration();
}

int game_sm_subsystem_get_next_state(struct GameStateMachineInput input,
                                     struct GameStateMachineState *data) {
  size_t counter;
  size_t i;
  int err;

  counter = *priv_ops.get_counter();

  for (i = 0; i < counter; i++) {
    logging_ops->log_info(game_sm_subsystem_module_id, "Processing %s",
                          game_sm_subsystem.registrations[i]->id);

    err = game_sm_subsystem.registrations[i]->next_state(input, data);

    if (err) {
      logging_ops->log_err(
          game_sm_subsystem_module_id, "Unable to process %s: %s",
          game_sm_subsystem.registrations[i]->id, strerror(err));
      return err;
    }
  }

  return 0;
};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int game_sm_subsystem_init(void) {
  logging_ops = get_logging_utils_ops();

  return 0;
}

void game_sm_subsystem_priority_handle_new_registration(void) {
  size_t counter = *priv_ops.get_counter();
  struct GameSmSubsystemRegistrationData *new_reg =
      game_sm_subsystem.registrations[counter - 1];

  if (new_reg->priority > 0)
    priv_ops.priority_handle_positive_value(new_reg);
  else if (new_reg->priority < 0)
    priv_ops.priority_handle_negative_value(new_reg);
  else // if (new_reg->priority == 0)
    priv_ops.priority_handle_no_value(new_reg);
}

void game_sm_subsystem_priority_handle_positive_value(
    struct GameSmSubsystemRegistrationData *new_reg) {
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t counter;
  size_t i;

  counter = *priv_ops.get_counter();

  for (i = 0; i < counter; i++) {
    tmp_reg = game_sm_subsystem.registrations[i];

    // If positive priority 1 is biggest value. The bigger
    //  number the smaller priority.
    if (
        // If comparing vs negative/no priority, write before.
        (tmp_reg->priority <= 0) ||
        // If comparing to positive priority with smaller number,
        //  write before tmp_reg.
        (new_reg->priority < tmp_reg->priority)) {
      priv_ops.insert_registration(i, new_reg);
      break;
    }
  }
}

void game_sm_subsystem_priority_handle_negative_value(
    struct GameSmSubsystemRegistrationData *new_reg) {
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t i = *priv_ops.get_counter();

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
      priv_ops.insert_registration(i + 1, new_reg);
      break;
    }
  }
}

void game_sm_subsystem_priority_handle_no_value(
    struct GameSmSubsystemRegistrationData *new_reg) {
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t i = *priv_ops.get_counter();

  for (i = 0; i < game_sm_subsystem.count; i++) {
    tmp_reg = game_sm_subsystem.registrations[i];

    if (new_reg->priority > tmp_reg->priority) {
      priv_ops.insert_registration(i, new_reg);
      break;
    }
  }
}

void game_sm_subsystem_insert_registration(
    int start, struct GameSmSubsystemRegistrationData *new_reg) {
  size_t i = *priv_ops.get_counter();

  // Move registrations by 1 right.
  while (i-- > start) {
    game_sm_subsystem.registrations[i + 1] = game_sm_subsystem.registrations[i];
  }

  game_sm_subsystem.registrations[start] = new_reg;
}

size_t *game_sm_subsystem_get_counter(void) {
  return &game_sm_subsystem.count;
};

static struct GameSmSubsystemRegistrationData **
game_sm_subsystem_get_registrations(void) {
  return game_sm_subsystem.registrations;
};

INIT_REGISTER_SUBSYSTEM_CHILD(&init_game_sm_sub_reg, init_game_reg_p);
