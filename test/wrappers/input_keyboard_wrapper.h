// C standard library
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#include "static_array_lib.h"

// App's internal libs
#include "input/input.h"
#include "input/input_common.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_keys_mapping.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define KEYBOARD_KEYS_MAPPINGS_MAX 10
#define KEYBOARD_STDIN_BUFFER_MAX 10

typedef struct KeyboardSubsystem {
  pthread_t thread;
  bool is_initialized;
  size_t stdin_buffer_count;
  char stdin_buffer[KEYBOARD_STDIN_BUFFER_MAX];
  SARRS_FIELD(keys_mappings, struct KeyboardKeysMapping,
              KEYBOARD_KEYS_MAPPINGS_MAX);
} KeyboardSubsystem;

SARRS_DECL(KeyboardSubsystem, keys_mappings, struct KeyboardKeysMapping,
           KEYBOARD_KEYS_MAPPINGS_MAX);

struct KeyboardPrivateOps {
  int (*init)(struct KeyboardSubsystem *);
  void (*destroy)(struct KeyboardSubsystem *);
  void (*read_stdin)(struct KeyboardSubsystem *);
  int (*start_thread)(struct KeyboardSubsystem *);
  void (*stop_thread)(struct KeyboardSubsystem *);
  void *(*process_stdin)(struct KeyboardSubsystem *);
  void (*execute_callbacks)(struct KeyboardSubsystem *);
  int (*add_keys_mapping)(struct KeyboardAddKeysMappingInput *,
                          struct KeyboardAddKeysMappingOutput *);
};

struct KeyboardPrivateOps *get_keyboard_priv_ops(void);
