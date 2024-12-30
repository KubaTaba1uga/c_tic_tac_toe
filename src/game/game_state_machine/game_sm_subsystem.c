/*******************************************************************************
 * @file game_sm_subsystem.c
 * @brief Game State Machine Subsystem Implementation
 *
 ******************************************************************************/
/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "static_array_lib.h"

// App's internal libs
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define GAME_SM_MINI_MACHINES_MAX 100

struct GameSmSubsystem {
  SARRS_FIELD(mini_machines, struct MiniGameStateMachine,
              GAME_SM_MINI_MACHINES_MAX);
};

typedef struct GameSmSubsystem GameSmSubsystem;

SARRS_DECL(GameSmSubsystem, mini_machines, struct MiniGameStateMachine,
           GAME_SM_MINI_MACHINES_MAX);

struct GameSmSubsystemPrivateOps {
  void (*priority_handle_new_registration)(void);
  void (*priority_handle_positive_value)(struct MiniGameStateMachine *new_reg);
  void (*priority_handle_negative_value)(struct MiniGameStateMachine *new_reg);
  void (*priority_handle_no_value)(struct MiniGameStateMachine *new_reg);
  void (*insert_registration)(int start, struct MiniGameStateMachine *new_reg);
  struct GameSmSubsystem *(*get_subsystem)(void);
};

static struct LoggingUtilsOps *logging_ops;
static struct GameSmSubsystem game_sm_subsystem;
static char game_sm_subsystem_module_id[] = "game_sm_subsystem";

static struct GameSmSubsystemPrivateOps *gsm_sub_priv_ops;
struct GameSmSubsystemPrivateOps *get_gsm_sub_private_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int game_sm_subsystem_init(void) {
  logging_ops = get_logging_utils_ops();

  gsm_sub_priv_ops = get_gsm_sub_private_ops();

  struct GameSmSubsystem *subsystem = gsm_sub_priv_ops->get_subsystem();
  GameSmSubsystem_mini_machines_init(subsystem);

  return 0;
}

int game_sm_subsystem_add_mini_state_machine(
    struct MiniGameStateMachine mini_state_machine) {
  struct GameSmSubsystem *subsystem = gsm_sub_priv_ops->get_subsystem();
  int err;

  if (!mini_state_machine.next_state || !mini_state_machine.display_name) {
    return EINVAL;
  }

  err = GameSmSubsystem_mini_machines_append(subsystem, mini_state_machine);
  if (err) {
    logging_ops->log_err(game_sm_subsystem_module_id,
                         "Unable to add mini state machine %s: %s",
                         mini_state_machine.display_name, strerror(err));
    return err;
  }

  gsm_sub_priv_ops->priority_handle_new_registration();

  return 0;
}

int game_sm_subsystem_get_next_state(struct GameStateMachineInput input,
                                     struct GameStateMachineState *data) {
  struct GameSmSubsystem *subsystem = gsm_sub_priv_ops->get_subsystem();
  struct MiniGameStateMachine *mini_state_machine;
  size_t i;
  int err;

  for (i = 0; i < GameSmSubsystem_mini_machines_length(subsystem); i++) {
    GameSmSubsystem_mini_machines_get(subsystem, i, &mini_state_machine);

    logging_ops->log_info(game_sm_subsystem_module_id, "Processing %s",
                          mini_state_machine->display_name);

    err = mini_state_machine->next_state(input, data);
    if (err) {
      logging_ops->log_err(game_sm_subsystem_module_id,
                           "Unable to process %s: %s",
                           mini_state_machine->display_name, strerror(err));
      return err;
    }
  }

  return 0;
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

void game_sm_subsystem_priority_handle_new_registration(void) {
  struct GameSmSubsystem *subsystem = gsm_sub_priv_ops->get_subsystem();
  struct MiniGameStateMachine *new_mini_sm;

  GameSmSubsystem_mini_machines_get(
      subsystem, GameSmSubsystem_mini_machines_length(subsystem) - 1,
      &new_mini_sm);

  if (new_mini_sm->priority > 0)
    gsm_sub_priv_ops->priority_handle_positive_value(new_mini_sm);
  else if (new_mini_sm->priority < 0)
    gsm_sub_priv_ops->priority_handle_negative_value(new_mini_sm);
  else // if (new_mini_sm->priority == 0)
    gsm_sub_priv_ops->priority_handle_no_value(new_mini_sm);
}

void game_sm_subsystem_priority_handle_positive_value(
    struct MiniGameStateMachine *new_mini_sm) {
  struct GameSmSubsystem *subsystem = gsm_sub_priv_ops->get_subsystem();
  struct MiniGameStateMachine *tmp_mini_sm;
  size_t i;

  for (i = 0; i < GameSmSubsystem_mini_machines_length(subsystem); i++) {
    GameSmSubsystem_mini_machines_get(subsystem, i, &tmp_mini_sm);

    if (
        // If comparing vs negative/no priority, write before.
        (tmp_mini_sm->priority <= 0) ||
        // If comparing to positive priority with smaller number,
        //  write before tmp_mini_sm.
        (new_mini_sm->priority < tmp_mini_sm->priority)) {
      gsm_sub_priv_ops->insert_registration(i, new_mini_sm);
      break;
    }
  }
}

void game_sm_subsystem_priority_handle_negative_value(
    struct MiniGameStateMachine *new_mini_sm) {
  struct GameSmSubsystem *subsystem = gsm_sub_priv_ops->get_subsystem();
  struct MiniGameStateMachine *tmp_mini_sm;
  size_t counter = GameSmSubsystem_mini_machines_length(subsystem);

  while (counter-- > 0) {
    GameSmSubsystem_mini_machines_get(subsystem, counter, &tmp_mini_sm);

    if (
        // If comparing vs positive/no priority, write after.
        (tmp_mini_sm->priority >= 0) ||
        // If comparing to negative priority with bigger number,
        //  write after tmp_mini_sm.
        (new_mini_sm->priority > tmp_mini_sm->priority)) {
      gsm_sub_priv_ops->insert_registration(counter + 1, new_mini_sm);
      break;
    }
  }
}

void game_sm_subsystem_priority_handle_no_value(
    struct MiniGameStateMachine *new_mini_sm) {
  struct GameSmSubsystem *subsystem = gsm_sub_priv_ops->get_subsystem();
  struct MiniGameStateMachine *tmp_mini_sm;
  size_t i;

  for (i = 0; i < GameSmSubsystem_mini_machines_length(subsystem); i++) {
    GameSmSubsystem_mini_machines_get(subsystem, i, &tmp_mini_sm);

    if (new_mini_sm->priority > tmp_mini_sm->priority) {
      gsm_sub_priv_ops->insert_registration(i, new_mini_sm);
      break;
    }
  }
}

void game_sm_subsystem_insert_registration(
    int start, struct MiniGameStateMachine *new_mini_sm) {
  struct GameSmSubsystem *subsystem = gsm_sub_priv_ops->get_subsystem();
  size_t counter = GameSmSubsystem_mini_machines_length(subsystem);

  // Move mini_machines by 1 right.
  while (counter-- > start) {
    subsystem->mini_machines[counter + 1] = subsystem->mini_machines[counter];
  }

  subsystem->mini_machines[start] = *new_mini_sm;
}

struct GameSmSubsystem *game_sm_subsystem_get_subsystem(void) {
  return &game_sm_subsystem;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct GameSmSubsystemOps game_sm_subsystem_ops = {
    .init = game_sm_subsystem_init,
    .next_state = game_sm_subsystem_get_next_state,
    .add_mini_state_machine = game_sm_subsystem_add_mini_state_machine,
};

struct GameSmSubsystemOps *get_game_sm_subsystem_ops(void) {
  return &game_sm_subsystem_ops;
};

static struct GameSmSubsystemPrivateOps gsm_sub_priv_ops_ = {
    .priority_handle_new_registration =
        game_sm_subsystem_priority_handle_new_registration,
    .priority_handle_positive_value =
        game_sm_subsystem_priority_handle_positive_value,
    .priority_handle_negative_value =
        game_sm_subsystem_priority_handle_negative_value,
    .priority_handle_no_value = game_sm_subsystem_priority_handle_no_value,
    .insert_registration = game_sm_subsystem_insert_registration,
    .get_subsystem = game_sm_subsystem_get_subsystem};

struct GameSmSubsystemPrivateOps *get_gsm_sub_private_ops(void) {
  return &gsm_sub_priv_ops_;
}
