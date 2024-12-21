#include "config/config.h"
#include "utils/registration_utils.h"

struct ConfigSubsystem {
  struct Registrar registrar;
};

struct ConfigPrivateOps {
  int (*init)(struct ConfigSubsystem *);
  void (*destroy)(struct ConfigSubsystem *);
  int (*get_variable)(struct ConfigGetVarInput *, struct ConfigGetVarOutput *);
  int (*register_variable)(struct ConfigRegisterVarInput *,
                           struct ConfigRegisterVarOutput *);
};

struct ConfigPrivateOps *get_config_priv_ops(void);
