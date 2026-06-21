//
// Created by Dwayne Edwards on 2026-06-21.
//

#ifndef GDTK_TYPES_H
#define GDTK_TYPES_H

#include <volk.h>

#include <SDL3/SDL_vulkan.h>

#include "gdtk/core/defines.h"
#include "gdtk/platform/log.h"

typedef void* WINDOW_HANDLE;

#define SDL_CHECK(x)                                                                                                   \
  ({                                                                                                                   \
    s32 result = (!(x));                                                                                               \
    b8 r = result == 0;                                                                                                \
    if (!r) LOG_ERROR("Error: %s", SDL_GetError());                                                                    \
    r;                                                                                                                 \
  })

#define VK_CHECK(x)                                                                                                    \
  ({                                                                                                                   \
    VkResult result = (x);                                                                                             \
    b8 r = result == VK_SUCCESS;                                                                                       \
    if (!r) LOG_ERROR("Vulkan Error: %d", result);                                                                     \
    r;                                                                                                                 \
  })

struct GDTK_Config;

typedef struct GDTK_Renderer
{
  WINDOW_HANDLE window;

  VkInstance instance;
  VkSurfaceKHR surface;

  VkPhysicalDeviceProperties physical_device_props;
  VkPhysicalDevice physical_device;
  VkDevice device;

  u32 graphics_queue_idx;
  u32 present_queue_idx;
  VkQueue graphics_queue;
  VkQueue present_queue;

  VkSwapchainKHR swapchain;
  u32 swapchain_image_count;
  VkImage* swapchain_images;
  VkImageView* swapchain_image_views;
  VkFormat swapchain_format;
  VkExtent2D swapchain_extent;
} GDTK_Renderer;

#endif // GDTK_TYPES_H
