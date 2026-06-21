//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_LOG_H
#define GDTK_LOG_H


typedef enum GDTK_LogPriority
{
  GDTK_LOG_PRIORITY_INVALID,
  GDTK_LOG_PRIORITY_TRACE,
  GDTK_LOG_PRIORITY_VERBOSE,
  GDTK_LOG_PRIORITY_DEBUG,
  GDTK_LOG_PRIORITY_INFO,
  GDTK_LOG_PRIORITY_WARN,
  GDTK_LOG_PRIORITY_ERROR,
  GDTK_LOG_PRIORITY_CRITICAL,
  GDTK_LOG_PRIORITY_COUNT
} GDTK_LogPriority;


#define STR(x) #x
#define TO_STR(x) STR(x)


#define LOG(priority, fmt, ...) log_message(priority, fmt " | " __FILE__ ":" TO_STR(__LINE__), ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) LOG(GDTK_LOG_PRIORITY_DEBUG, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) LOG(GDTK_LOG_PRIORITY_INFO, fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) LOG(GDTK_LOG_PRIORITY_WARN, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) LOG(GDTK_LOG_PRIORITY_ERROR, fmt, ##__VA_ARGS__)

void
log_message(GDTK_LogPriority priority, const char* fmt, ...);

#endif // GDTK_LOG_H
