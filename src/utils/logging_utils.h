#ifndef _LOGGING_UTILS_H
#define _LOGGING_UTILS_H

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct logging_utils_ops {
  int (*init_loggers)(void);
  void (*destroy_loggers)(void);
  void (*log_info)(const char *msg_id, char *fmt, ...);
  void (*log_err)(const char *msg_id, char *fmt, ...);
  void *private;
};

extern struct logging_utils_ops logging_utils_ops;

#endif // _LOGGING_UTILS_H
