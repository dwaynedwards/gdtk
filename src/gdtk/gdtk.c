//
// Created by Dwayne Edwards on 2026-06-20.
//


#include "gdtk/gdtk.h"

#include <SDL3/SDL_main.h>

#include "gdtk/platform/log.h"

#ifndef GDTK_LOG_PRIORITY
#define GDTK_LOG_PRIORITY GDTK_LOG_PRIORITY_ERROR
#endif


void
log_init();
void
set_log_priority(GDTK_LogPriority priority);

SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[])
{
  set_log_priority(_Generic((GDTK_LOG_PRIORITY), int: (GDTK_LOG_PRIORITY), default: GDTK_LOG_PRIORITY_INFO));

  log_init();

  LOG_INFO("Starting up GDTK");

  const SDL_AppResult result = (SDL_AppResult)gdtk_startup(appstate, argc, argv);

  LOG_INFO("GDTK started");

  return result;
}

SDL_AppResult
SDL_AppIterate(void* appstate)
{
  return (SDL_AppResult)gdtk_iterate(appstate);
}

SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event)
{
  switch (event->type)
  {
    case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
    default: break;
  }

  return (SDL_AppResult)gdtk_events(appstate, (GDTK_Event*)event);
}

void
SDL_AppQuit(void* appstate, SDL_AppResult result)
{
  LOG_INFO("Shutting down GDTK");

  gdtk_shutdown(appstate, (GDTK_AppResult)result);

  LOG_INFO("GDTK shutdown");
}
