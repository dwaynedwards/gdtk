//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_APP_H
#define GDTK_APP_H

#include "gdtk/core/types/vector.h"

typedef struct GDTK_Config
{
  const char* title;
  sVec2 dimensions;
} GDTK_Config;

typedef struct GDTK_App GDTK_App;

GDTK_App*
app_create(const GDTK_Config* cfg);

void
app_destroy(GDTK_App* app);

#endif // GDTK_APP_H
