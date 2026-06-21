//
// Created by Dwayne Edwards on 2026-06-21.
//

#ifndef GDTK_SURFACE_H
#define GDTK_SURFACE_H

#include "gdtk/graphics/vulkan/types.h"

b8
create_surface(GDTK_Renderer* renderer, const struct GDTK_Config* cfg);

void
destroy_surface(GDTK_Renderer* renderer);

#endif // GDTK_SURFACE_H
