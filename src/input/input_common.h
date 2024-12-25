#ifndef INPUT_COMMON_H
#define INPUT_COMMON_H

enum InputEvents {
  INPUT_EVENT_NONE = 0,
  INPUT_EVENT_UP,
  INPUT_EVENT_DOWN,
  INPUT_EVENT_LEFT,
  INPUT_EVENT_RIGHT,
  INPUT_EVENT_SELECT,
  INPUT_EVENT_EXIT,
  INPUT_EVENT_INVALID,
};

typedef int input_device_id_t;

typedef int (*input_wait_func_t)(void);
typedef int (*input_stop_func_t)(void);
typedef int (*input_start_func_t)(void);
typedef int (*input_callback_func_t)(enum InputEvents, input_device_id_t);

#endif
