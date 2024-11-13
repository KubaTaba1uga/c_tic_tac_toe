/*******************************************************************************
 * @file game_sm_subsystem.c
 * @brief Game State Machine Subsystem Implementation
 *
 * This file implements the core functionality of the Game State Machine (SM)
 * subsystem, which is responsible for managing and transitioning between
 * various states within the game's state machine architecture. The Game SM
 * subsystem maintains an internal list of state machine registrations, each
 * representing a unique state with a specific priority level and handling
 * logic.
 *
 * The primary responsibilities of this module include:
 *  - Registering state machines into a prioritized list.
 *  - Processing game state transitions based on specific inputs.
 *  - Managing priority-based insertion and ordering of state machine
 *    registrations.
 *  - Defining internal operations for modular access to subsystem
 *    functionalities.
 *
 * This module interacts closely with other parts of the game through a public
 * API and modular private operations, allowing controlled access to subsystem
 * components and behaviors. Logging and initialization utilities are also
 * utilized for error handling and subsystem setup.
 *
 * @note This file is part of the game state machine module and should be used
 *       within the context of the larger game application.
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
static size_t game_sm_subsystem_get_counter_copy(void);
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
  size_t (*get_counter_copy)(void);
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
    .get_counter_copy = game_sm_subsystem_get_counter_copy,
    .get_registrations = game_sm_subsystem_get_registrations};

static struct GameSmSubsystemOps game_sm_subsystem_ops = {
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
  struct GameSmSubsystemRegistrationData **registrations;
  size_t *counter;

  registrations = priv_ops.get_registrations();
  counter = priv_ops.get_counter();

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
  struct GameSmSubsystemRegistrationData **registrations;
  size_t counter;
  size_t i;
  int err;

  registrations = priv_ops.get_registrations();
  counter = priv_ops.get_counter_copy();

  for (i = 0; i < counter; i++) {
    logging_ops->log_info(game_sm_subsystem_module_id, "Processing %s",
                          registrations[i]->id);

    err = registrations[i]->next_state(input, data);

    if (err) {
      logging_ops->log_err(game_sm_subsystem_module_id,
                           "Unable to process %s: %s", registrations[i]->id,
                           strerror(err));
      return err;
    }
  }

  return 0;
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int game_sm_subsystem_init(void) {
  logging_ops = get_logging_utils_ops();

  return 0;
}

void game_sm_subsystem_priority_handle_new_registration(void) {
  struct GameSmSubsystemRegistrationData **registrations;
  struct GameSmSubsystemRegistrationData *new_reg;
  size_t counter;

  registrations = priv_ops.get_registrations();
  counter = priv_ops.get_counter_copy();
  new_reg = registrations[counter - 1];

  if (new_reg->priority > 0)
    priv_ops.priority_handle_positive_value(new_reg);
  else if (new_reg->priority < 0)
    priv_ops.priority_handle_negative_value(new_reg);
  else // if (new_reg->priority == 0)
    priv_ops.priority_handle_no_value(new_reg);
}

void game_sm_subsystem_priority_handle_positive_value(
    struct GameSmSubsystemRegistrationData *new_reg) {
  struct GameSmSubsystemRegistrationData **registrations;
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t counter;
  size_t i;

  registrations = priv_ops.get_registrations();
  counter = priv_ops.get_counter_copy();

  for (i = 0; i < counter; i++) {
    tmp_reg = registrations[i];

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
  struct GameSmSubsystemRegistrationData **registrations;
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t counter;

  registrations = priv_ops.get_registrations();
  counter = priv_ops.get_counter_copy();

  while (counter-- > 0) {
    tmp_reg = registrations[counter];

    if (
        // If comparing vs positive/no priority, write after.
        (tmp_reg->priority >= 0) ||
        // If comparing to negative priority with bigger number,
        //  write after tmp_reg.
        (new_reg->priority > tmp_reg->priority)) {
      priv_ops.insert_registration(counter + 1, new_reg);
      break;
    }
  }
}

void game_sm_subsystem_priority_handle_no_value(
    struct GameSmSubsystemRegistrationData *new_reg) {
  struct GameSmSubsystemRegistrationData **registrations;
  struct GameSmSubsystemRegistrationData *tmp_reg;
  size_t counter;
  size_t i;

  registrations = priv_ops.get_registrations();
  counter = priv_ops.get_counter_copy();

  for (i = 0; i < counter; i++) {
    tmp_reg = registrations[i];

    if (new_reg->priority > tmp_reg->priority) {
      priv_ops.insert_registration(i, new_reg);
      break;
    }
  }
}

void game_sm_subsystem_insert_registration(
    int start, struct GameSmSubsystemRegistrationData *new_reg) {
  struct GameSmSubsystemRegistrationData **registrations;
  size_t counter;

  registrations = priv_ops.get_registrations();
  counter = priv_ops.get_counter_copy();

  // Move registrations by 1 right.
  while (counter-- > start) {
    registrations[counter + 1] = registrations[counter];
  }

  registrations[start] = new_reg;
}

size_t *game_sm_subsystem_get_counter(void) {
  return &game_sm_subsystem.count;
};

size_t game_sm_subsystem_get_counter_copy(void) {
  return game_sm_subsystem.count;
};

static struct GameSmSubsystemRegistrationData **
game_sm_subsystem_get_registrations(void) {
  return game_sm_subsystem.registrations;
};

INIT_REGISTER_SUBSYSTEM_CHILD(&init_game_sm_sub_reg, init_game_reg_p);
