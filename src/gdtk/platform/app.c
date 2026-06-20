//
// Created by Dwayne Edwards on 2026-06-20.
//

#include "gdtk/platform/app.h"

#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"

#include "gdtk/platform/log.h"


#define SDL_CHECK(x)                                                                                                   \
  ({                                                                                                                   \
    S32 result = (!(x));                                                                                               \
    bool r = result == 0;                                                                                              \
    if (!r) LOG_ERROR("Error: %s", SDL_GetError());                                                                    \
    r;                                                                                                                 \
  })


GDTK_App*
app_create(const GDTK_Config* cfg)
{
  GDTK_App* ctx = SDL_malloc(sizeof(GDTK_App));
  if (!ctx)
  {
    LOG_ERROR("Failed to allocate GDTK_Context memory");
    return NULL;
  }
  SDL_memset(ctx, 0, sizeof(GDTK_App));

  const SDL_WindowFlags flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  ctx->handle = SDL_CreateWindow(cfg->title, cfg->dimensions.w, cfg->dimensions.h, flags);
  if (!SDL_CHECK(ctx->handle))
  {
    app_destroy(ctx);
    return NULL;
  }

  return ctx;
}

void
app_destroy(GDTK_App* ctx)
{
  if (!ctx)
  {
    return;
  }

  LOG_INFO("Destroying GDTK_App");

  if (ctx->handle)
  {
    SDL_DestroyWindow(ctx->handle);
    ctx->handle = NULL;
  }

  SDL_free(ctx);
  ctx = NULL;

  LOG_INFO("GDTK_App destroyed");
}
