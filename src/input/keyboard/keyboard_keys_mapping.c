#include "errno.h"

#include "input/keyboard/keyboard_keys_mapping.h"

static int keyboard_keys_mapping_init(struct KeyboardKeysMapping *mapping,
                                      keyboard_key_mapping_callback_t callback,
                                      const char *display_name) {
  if (!mapping || !callback || !display_name) {
    return EINVAL;
  }

  mapping->callback = callback;
  mapping->display_name = display_name;

  return 0;
}

static struct KeyboardKeysMappingOps keys_mapping_ops = {
    .init_keys_mapping = keyboard_keys_mapping_init,
};

struct KeyboardKeysMappingOps *get_keyboard_keys_mapping_ops(void) {
  return &keys_mapping_ops;
}
