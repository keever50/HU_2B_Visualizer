/*
 * @file log.c
 *
 *  Created on: Nov 14, 2024
 *      Author: kevin
 */

#include "log.h"

void _log_debug(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[32m[DEBUG]\033[0m %s:%d(%s): %s\r\n", file, line, func, msg);
  log_out(location_msg);
}

void _log_info(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "[INFO] %s: %s\r\n", func, msg);
  log_out(location_msg);
}

void _log_notice(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[34m[NOTICE]\033[0m %s: %s\r\n", func, msg);
  log_out(location_msg);
}

void _log_warning(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[33m[WARNING]\033[0m %s: %s\r\n", func, msg);
  log_out(location_msg);
}

void _log_error(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[31m[ERROR]\033[0m %s:%d(%s): %s\r\n", file, line, func, msg);
  log_out(location_msg);
}

void _log_critical(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[31m\033[5m[CRITICAL]\033[0m %s:%d(%s): %s\r\n", file, line, func, msg);
  log_out(location_msg);
}

void log_setlevel(enum log_level lvl)
{

}

void _log(enum log_level lvl, const char *msg, const char *file, const char *func, int line, ...)
{
  char msgbuff[LOG_MAX_MSG_SIZE];

  va_list vargs;
  va_start(vargs, line);
  vsnprintf(msgbuff, LOG_MAX_MSG_SIZE, msg, vargs);
  va_end(vargs);

  switch(lvl)
  {
    case LOG_DEBUG:
      _log_debug(msgbuff, file, func, line);
      break;
    case LOG_INFO:
      _log_info(msgbuff, file, func, line);
      break;
    case LOG_NOTICE:
      _log_notice(msgbuff, file, func, line);
      break;
    case LOG_WARNING:
      _log_warning(msgbuff, file, func, line);
      break;
    case LOG_ERROR:
      _log_error(msgbuff, file, func, line);
      break;
    case LOG_CRITICAL:
      _log_critical(msgbuff, file, func, line);
      break;
  }


  return;
}
