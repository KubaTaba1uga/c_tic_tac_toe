#include <pthread.h> // For pthread_t and thread management
#include <stdbool.h> // For bool type
#include <stddef.h>  // For size_t type

// App's internal libs
#include "input/keyboard/keyboard.h"

#define KEYBOARD_CALLBACK_MAX 10
#define KEYBOARD_STDIN_BUFFER_MAX 10

struct KeyboardSubsystem {
  pthread_t thread;
  bool is_initialized;
  struct Registrar registrar;
  // Stdin buffer is not using array abstraction as array
  //   abstraction uses void * to provide genericness.
  //   We want to store pure chars in the array no ptrs to chars.
  size_t stdin_buffer_count;
  char stdin_buffer[KEYBOARD_STDIN_BUFFER_MAX];
};

struct KeyboardPrivateOps {
  int (*init)(struct KeyboardSubsystem *);
  void (*destroy)(struct KeyboardSubsystem *);
  void (*read_stdin)(struct KeyboardSubsystem *);
  int (*start_thread)(struct KeyboardSubsystem *);
  void (*stop_thread)(struct KeyboardSubsystem *);
  void *(*process_stdin)(struct KeyboardSubsystem *);
  void (*execute_callbacks)(struct KeyboardSubsystem *);
  int (*register_callback)(struct KeyboardRegisterInput *,
                           struct KeyboardRegisterOutput *);
};

struct KeyboardPrivateOps *get_keyboard_priv_ops(void);
