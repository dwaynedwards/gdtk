//
// Created by Dwayne Edwards on 2026-06-20.
//

#include "gdtk/platform/log.h"

#include <SDL3/SDL_log.h>

#include "gdtk/core/defines.h"

void
set_log_priority(const GDTK_LogPriority priority)
{
  SDL_SetLogPriority(SDL_LOG_CATEGORY_CUSTOM, (SDL_LogPriority)priority);
}

static const char*
internal_get_priority_string(const SDL_LogPriority priority)
{
  switch (priority)
  {
    case SDL_LOG_PRIORITY_VERBOSE: return "VERBOSE";
    case SDL_LOG_PRIORITY_DEBUG: return "DEBUG";
    case SDL_LOG_PRIORITY_INFO: return "INFO";
    case SDL_LOG_PRIORITY_WARN: return "WARN";
    case SDL_LOG_PRIORITY_ERROR: return "ERROR";
    case SDL_LOG_PRIORITY_CRITICAL: return "CRITICAL";
    default: return "UNKNOWN";
  }
}

static void
internal_log_message_callback(void* userdata, s32 category, const SDL_LogPriority priority, const char* message)
{
  printf("[%s] %s\n", internal_get_priority_string(priority), message);
}

void
log_init()
{
  SDL_SetLogOutputFunction(internal_log_message_callback, NULL);
}

void
log_message(const GDTK_LogPriority priority, const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  SDL_LogMessageV(SDL_LOG_CATEGORY_CUSTOM, (SDL_LogPriority)priority, fmt, args);
  va_end(args);
}
