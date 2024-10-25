#include "game/game_state_machine/game_state_machine.h"

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
