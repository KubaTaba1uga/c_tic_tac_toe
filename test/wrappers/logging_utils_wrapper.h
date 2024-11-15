#include <stumpless.h>

struct LoggingUtilsPrivateOps {
  void (*log_msg)(char *msg, const char *msg_id,
                  enum stumpless_severity severity);
  int (*create_log_entry)(char *msg, const char *msg_id,
                          struct stumpless_entry **entry,
                          enum stumpless_severity severity);
  int (*emmit_log_entry)(struct stumpless_entry *entry);
  void (*print_errno)(void);
  void (*print_error)(char *error);
};
