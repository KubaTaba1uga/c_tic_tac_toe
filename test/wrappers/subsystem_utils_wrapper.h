#include <stdbool.h>

#include "utils/array_utils.h"
#include "utils/subsystem_utils.h"

struct Subsystem {
  const char *id;
  array_t registrations;
};

struct Module {
  const char *id;
  void *private;
};

struct ModuleSearchWrapper {
  bool (*filter_func_wrap)(const char *, void *, void *);
  array_search_t search_wrap;
  void *filter_data_wrap;
};

struct SubsystemUtilsPrivateOps {
  bool (*filter_func)(module_t, module_search_t);
};

struct SubsystemUtilsPrivateOps *get_subsystem_utils_priv_ops(void);
