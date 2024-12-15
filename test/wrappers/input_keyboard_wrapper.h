#include <pthread.h> // For pthread_t and thread management
#include <stdbool.h> // For bool type
#include <stddef.h>  // For size_t type

#include "input/keyboard/keyboard_registration.h"
#include "utils/subsystem_utils.h"

#define KEYBOARD_CALLBACK_MAX 10
#define KEYBOARD_STDIN_BUFFER_MAX 10

struct KeyboardSubsystem {
  pthread_t thread;
  bool is_initialized;
  subsystem_t subsystem;
  size_t stdin_buffer_count;
  // Stdin buffer is not using array abstraction as array
  //   abstraction uses void * to provide genericness.
  //   We want to store pure chars in the array no ptrs to chars.
  char stdin_buffer[KEYBOARD_STDIN_BUFFER_MAX];
};

typedef struct KeyboardSubsystem *keyboard_sys_t;

struct KeyboardPrivateOps {
  int (*init)(keyboard_sys_t *);
  void (*destroy)(keyboard_sys_t *);
  void (*read_stdin)(keyboard_sys_t);
  int (*start_thread)(keyboard_sys_t);
  void (*stop_thread)(keyboard_sys_t);
  void *(*process_stdin)(keyboard_sys_t);
  void (*execute_callbacks)(keyboard_sys_t);
  int (*register_callback)(keyboard_sys_t, keyboard_reg_t);
};

struct KeyboardPrivateOps *get_keyboard_priv_ops(void);
