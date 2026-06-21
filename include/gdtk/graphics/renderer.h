//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_RENDERER_H
#define GDTK_RENDERER_H


struct GDTK_Config;

typedef struct GDTK_Renderer GDTK_Renderer;

GDTK_Renderer*
renderer_create(const struct GDTK_Config* cfg);

void
renderer_destroy(GDTK_Renderer* renderer);

#endif // GDTK_RENDERER_H
