#ifndef INPUT_COMMON
#define INPUT_COMMON

enum InputEvents {
  INPUT_EVENT_NONE = 0,
  INPUT_EVENT_UP,
  INPUT_EVENT_DOWN,
  INPUT_EVENT_LEFT,
  INPUT_EVENT_RIGHT,
  INPUT_EVENT_SELECT,
  INPUT_EVENT_EXIT,
  INPUT_EVENT_MAX,
};

typedef int (*input_callback_func_t)(enum InputEvents input_event);

#endif // INPUT_COMMON
