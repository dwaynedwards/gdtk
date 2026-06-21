//
// Created by Dwayne Edwards on 2026-06-21.
//

#include "gdtk/graphics/vulkan/surface.h"

#include "gdtk/platform/app.h"

b8
create_surface(GDTK_Renderer* renderer, const GDTK_Config* cfg)
{
  LOG_INFO("Creating VkSurfaceKHR");

  const SDL_WindowFlags flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  renderer->window = SDL_CreateWindow(cfg->title, cfg->dimensions.w, cfg->dimensions.h, flags);
  if (!SDL_CHECK(renderer->window))
  {
    return false;
  }

  if (!SDL_CHECK(SDL_Vulkan_CreateSurface(renderer->window, renderer->instance, NULL, &renderer->surface)))
  {
    return false;
  }

  LOG_INFO("VkSurfaceKHR created");

  return true;
}

void
destroy_surface(GDTK_Renderer* renderer)
{
  if (renderer->surface)
  {
    vkDestroySurfaceKHR(renderer->instance, renderer->surface, NULL);
    renderer->surface = NULL;
  }

  if (renderer->window)
  {
    SDL_DestroyWindow(renderer->window);
    renderer->window = NULL;
  }
}
