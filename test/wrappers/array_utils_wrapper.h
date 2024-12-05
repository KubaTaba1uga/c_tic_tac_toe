#include "utils/array_utils.h"

struct ArrayPrivateOps {
  void (*increment_length)(array_t);
  size_t (*get_size)(array_t);
};

struct ArrayPrivateOps *get_array_priv_ops(void);
