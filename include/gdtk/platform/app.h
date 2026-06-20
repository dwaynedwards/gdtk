//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_APP_H
#define GDTK_APP_H


#include "types.h"


typedef struct
{
  const char* title;
  SVec2 dimensions;
} GDTK_Config;

typedef struct
{
  void* handle;
} GDTK_App;

GDTK_App*
app_create(const GDTK_Config* cfg);

void
app_destroy(GDTK_App* ctx);

#endif // GDTK_APP_H
