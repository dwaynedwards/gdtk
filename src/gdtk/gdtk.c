//
// Created by Dwayne Edwards on 2026-06-20.
//


#include "gdtk/gdtk.h"

#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>

#include "gdtk/platform/errors.h"
#include "gdtk/platform/log.h"

#ifndef GDTK_LOG_PRIORITY
#define GDTK_LOG_PRIORITY GDTK_LOG_PRIORITY_ERROR
#endif

#define PAUSE_RENDERING_SLEEP_TIME 100

void
log_init();
void
set_log_priority(GDTK_LogPriority priority);

static bool pause_rendering;

SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[])
{
  set_log_priority(_Generic((GDTK_LOG_PRIORITY), int: (GDTK_LOG_PRIORITY), default: GDTK_LOG_PRIORITY_INFO));

  log_init();

  LOG_INFO("Starting up GDTK Engine");

  if (!SDL_CHECK(SDL_Init(SDL_INIT_VIDEO)))
  {
    return SDL_APP_FAILURE;
  }

  const SDL_AppResult result = (SDL_AppResult)gdtk_startup(appstate, argc, argv);

  LOG_INFO("GDTK Engine started");

  return result;
}

SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event)
{
  switch (event->type)
  {
    case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_MINIMIZED:
    {
      pause_rendering = true;
      break;
    }
    case SDL_EVENT_WINDOW_RESIZED:
    {
      pause_rendering = false;
      break;
    }
    default: break;
  }

  return (SDL_AppResult)gdtk_events(appstate, (GDTK_Event*)event);
}

SDL_AppResult
SDL_AppIterate(void* appstate)
{
  if (pause_rendering)
  {
    SDL_Delay(PAUSE_RENDERING_SLEEP_TIME);
    return SDL_APP_CONTINUE;
  }

  return (SDL_AppResult)gdtk_iterate(appstate);
}

void
SDL_AppQuit(void* appstate, SDL_AppResult result)
{
  LOG_INFO("Shutting down GDTK Engine");

  gdtk_shutdown(appstate, (GDTK_AppResult)result);

  LOG_INFO("GDTK Engine shutdown");

  SDL_Quit();
}
