#ifndef _logging_utils_h
#define _logging_utils_h

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
void log_info(char *msg_id, char *fmt, ...);
int init_loggers(void);
void destroy_loggers(void);
#endif
