//
// Created by Dwayne Edwards on 2026-06-20.
//

#include "gdtk/graphics/renderer.h"

#if GDTK_PLATFORM_APPLE

#include "gdtk/graphics/vulkan/vulkan.h"

GDTK_Renderer*
renderer_create(const struct GDTK_Config* cfg)
{
  GDTK_Renderer* renderer = SDL_malloc(sizeof(GDTK_Renderer));
  if (!renderer)
  {
    LOG_ERROR("Failed to allocate GDTK_Renderer memory");
    renderer_destroy(renderer);
    return NULL;
  }
  SDL_memset(renderer, 0, sizeof(GDTK_Renderer));

  if (!create_instance(renderer, cfg))
  {
    LOG_ERROR("Failed to create VkInstance");
    renderer_destroy(renderer);
    return NULL;
  }

  if (!create_surface(renderer, cfg))
  {
    LOG_ERROR("Failed to create VkSurfaceKHR");
    renderer_destroy(renderer);
    return NULL;
  }

  if (!create_device(renderer))
  {
    LOG_ERROR("Failed to create Device");
    renderer_destroy(renderer);
    return NULL;
  }

  if (!create_swapchain(renderer))
  {
    LOG_ERROR("failed to create Swapchain");
    renderer_destroy(renderer);
    return NULL;
  }

  return renderer;
}

void
renderer_destroy(GDTK_Renderer* renderer)
{
  if (!renderer)
  {
    return;
  }

  LOG_INFO("Destroying GDTK_Renderer");

  if (renderer->device)
  {
    vkDeviceWaitIdle(renderer->device);
  }

  destroy_swapchain(renderer);
  destroy_device(renderer);
  destroy_surface(renderer);
  destroy_instance(renderer);

  SDL_free(renderer);
  renderer = NULL;

  LOG_INFO("GDTK_Renderer destroyed");
}

#endif
