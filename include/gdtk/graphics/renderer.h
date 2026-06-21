//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_RENDERER_H
#define GDTK_RENDERER_H

#include "gdtk/core/defines.h"

struct GDTK_Config;

struct GDTK_Renderer;

struct GDTK_Renderer*
renderer_create(const struct GDTK_Config* cfg);

void
renderer_destroy(struct GDTK_Renderer* renderer);

#endif // GDTK_RENDERER_H
