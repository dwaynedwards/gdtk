//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_ERRORS_H
#define GDTK_ERRORS_H

#define SDL_CHECK(x)                                                                                                   \
  ({                                                                                                                   \
    S32 result = (!(x));                                                                                               \
    bool r = result == 0;                                                                                              \
    if (!r) LOG_ERROR("Error: %s", SDL_GetError());                                                                    \
    r;                                                                                                                 \
  })

#define VK_CHECK(x)                                                                                                    \
  ({                                                                                                                   \
    VkResult result = (x);                                                                                             \
    bool r = result == VK_SUCCESS;                                                                                     \
    if (!r) LOG_ERROR("Vulkan Error: %d", result);                                                                     \
    r;                                                                                                                 \
  })

#endif // GDTK_ERRORS_H
