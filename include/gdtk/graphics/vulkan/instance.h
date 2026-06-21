//
// Created by Dwayne Edwards on 2026-06-21.
//

#ifndef GDTK_INSTANCE_H
#define GDTK_INSTANCE_H

#include "gdtk/graphics/vulkan/types.h"

b8
create_instance(GDTK_Renderer* renderer, const struct GDTK_Config* cfg);

void
destroy_instance(GDTK_Renderer* renderer);

#endif // GDTK_INSTANCE_H
