/*******************************************************************************
 * @file logging_utils.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <string.h>

// Logging library
#include <stdlib.h>
#include <stumpless.h>

#include "static_array_lib.h"

// App's internal libs
#include "config/config.h"
#include "init/init.h"
#include "stumpless/target.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define GET_VA_CHAR_ARGS(buffer, buffer_size, fmt)                             \
  va_list vl;                                                                  \
  va_start(vl, fmt);                                                           \
  vsnprintf(buffer, buffer_size, fmt, vl);                                     \
  va_end(vl);

#define LOGGING_MODULE_ID "logging_subsystem"

struct LoggingUtilsPrivateOps {
  int (*init_console_log)(void);
  int (*init_file_log)(void);
  void (*log_msg)(char *msg, const char *msg_id,
                  enum stumpless_severity severity);
  int (*create_log_entry)(char *msg, const char *msg_id,
                          struct stumpless_entry **entry,
                          enum stumpless_severity severity);
  int (*emmit_log_entry)(struct stumpless_entry *entry);
  void (*print_errno)(void);
  void (*print_error)(char *error);
};

struct LoggingSubsystem {
  struct stumpless_target *console_logger;
  struct stumpless_target *file_logger;
};

struct LoggingSubsystem logging_subsystem;
static struct LoggingUtilsPrivateOps *logging_utils_priv_ops;
static struct LoggingUtilsOps *logging_utils_ops;
struct LoggingUtilsPrivateOps *get_logging_utils_private_ops(void);

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int init_loggers(void) {
  int err;
  logging_utils_priv_ops = get_logging_utils_private_ops();
  logging_utils_ops = get_logging_utils_ops();

  err = logging_utils_priv_ops->init_console_log();
  if (err) {
    return err;
  }

  err = logging_utils_priv_ops->init_file_log();
  if (err) {
    return err;
  }

  // Register cleanup at program exit
  atexit(logging_utils_ops->destroy);

  return errno;
}

static int init_console_logger(void) {
  struct stumpless_target *tmp_logger;

  errno = 0;
  tmp_logger = stumpless_open_stdout_target("console logger");
  if (!tmp_logger) {
    logging_utils_priv_ops->print_error("Unable to open console logger");
    return errno;
  }

  logging_subsystem.console_logger = tmp_logger;

  return 0;
}

static int init_file_logger(void) {
  struct stumpless_target *tmp_logger;

  errno = 0;
  tmp_logger = stumpless_open_file_target("tic_tac_toe.log");

  if (!tmp_logger) {
    logging_utils_priv_ops->print_error(
        "Unable to open tic_tac_toe.log logger");
    return EINVAL;
  }

  logging_subsystem.file_logger = tmp_logger;

  return 0;
}

void destroy_loggers(void) {
  struct stumpless_target **loggers[] = {&logging_subsystem.console_logger,
                                         &logging_subsystem.file_logger};
  size_t i;

  for (i = 0; i < sizeof(loggers) / sizeof(struct stumpless_target *); i++) {
    if (*loggers[i] == NULL)
      continue;

    stumpless_close_target(*loggers[i]);

    *loggers[i] = NULL;
  }

  stumpless_free_all();
}

void disable_console_logger(void) {
  // Console logger is last logger
  if (!logging_subsystem.console_logger)
    return;

  stumpless_close_target(logging_subsystem.console_logger);
  logging_subsystem.console_logger = NULL;
};

void log_info(const char *msg_id, char *fmt, ...) {
  char local_log_entry[255];

  GET_VA_CHAR_ARGS(local_log_entry, 254, fmt);

  logging_utils_priv_ops->log_msg(local_log_entry, msg_id,
                                  STUMPLESS_SEVERITY_INFO);
}

void log_err(const char *msg_id, char *fmt, ...) {
  char local_log_entry[255];

  GET_VA_CHAR_ARGS(local_log_entry, sizeof(local_log_entry) / sizeof(char),
                   fmt);

  logging_utils_priv_ops->log_msg(local_log_entry, msg_id,
                                  STUMPLESS_SEVERITY_ERR);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
void log_msg(char *msg, const char *msg_id, enum stumpless_severity severity) {
  struct stumpless_entry *entry = NULL;
  int err;
  /* logging_utils_priv_ops->print_error("Test error printing"); */

  err = logging_utils_priv_ops->create_log_entry(msg, msg_id, &entry, severity);

  if (err) {
    logging_utils_priv_ops->print_error("Unable to create log entry");
    goto OUT;
  }

  err = logging_utils_priv_ops->emmit_log_entry(entry);

  if (err) {
    logging_utils_priv_ops->print_error("Unable to emmit log entry");
    goto FREE;
  }

FREE:
  stumpless_destroy_entry_and_contents(entry);

OUT:

  return;
}

int create_log_entry(char *msg, const char *msg_id,
                     struct stumpless_entry **entry,
                     enum stumpless_severity severity) {
  *entry = stumpless_new_entry_str(STUMPLESS_FACILITY_USER, severity,
                                   PROJECT_NAME, msg_id, msg);
  if (*entry == NULL) {
    return EINVAL;
  }

  return 0;
}

int emmit_log_entry(struct stumpless_entry *entry) {
  struct stumpless_target *loggers[] = {logging_subsystem.console_logger,
                                        logging_subsystem.file_logger};
  size_t i;
  int err;

  for (i = 0; i < sizeof(loggers) / sizeof(struct stumpless_target *); i++) {
    if (loggers[i] == NULL)
      continue;

    err = stumpless_add_entry(loggers[i], entry);

    if (err < 0) {
      return EINVAL;
    }
  }

  return 0;
}

void print_error(char *msg) {
  fprintf(stderr, "!!! Logging Error: %s !!!\n", msg);
  logging_utils_priv_ops->print_errno();
}

void print_errno(void) { stumpless_perror("logging"); }

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct LoggingUtilsPrivateOps priv_ops = {
    .log_msg = log_msg,
    .create_log_entry = create_log_entry,
    .emmit_log_entry = emmit_log_entry,
    .print_errno = print_errno,
    .print_error = print_error,
    .init_console_log = init_console_logger,
    .init_file_log = init_file_logger,
};

static struct LoggingUtilsOps pub_ops = {
    .init = init_loggers,
    .destroy = destroy_loggers,
    .log_info = log_info,
    .log_err = log_err,
    .disable_console_logger = disable_console_logger,
};

struct LoggingUtilsOps *get_logging_utils_ops(void) {
  return &pub_ops;
}

struct LoggingUtilsPrivateOps *get_logging_utils_private_ops(void) {
  return &priv_ops;
}
