//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_H
#define GDTK_H

#include "gdtk/core/defines.h"
#include "gdtk/platform/app.h"
#include "gdtk/platform/events.h"
#include "gdtk/platform/log.h"

typedef enum
{
  GDTK_APP_CONTINUE,
  GDTK_APP_SUCCESS,
  GDTK_APP_FAILURE,
} GDTK_AppResult;

extern GDTK_AppResult
gdtk_startup(void** gdtk_state, int argc, char* argv[]);

extern GDTK_AppResult
gdtk_events(void* gdtk_state, const GDTK_Event* events);

extern GDTK_AppResult
gdtk_iterate(void* gdtk_state);

extern void
gdtk_shutdown(void* gdtk_state, GDTK_AppResult result);

#endif // GDTK_H
