//
// Created by Dwayne Edwards on 2026-06-20.
//

#include "gdtk/platform/app.h"

#include "SDL3/SDL_stdinc.h"

#include "gdtk/graphics/renderer.h"
#include "gdtk/platform/log.h"

struct GDTK_App
{
  struct GDTK_Renderer* renderer;
};

GDTK_App*
app_create(const GDTK_Config* cfg)
{
  GDTK_App* app = SDL_malloc(sizeof(GDTK_App));
  if (!app)
  {
    LOG_ERROR("Failed to allocate GDTK_Context memory");
    return NULL;
  }
  SDL_memset(app, 0, sizeof(GDTK_App));

  app->renderer = renderer_create(cfg);
  if (!app->renderer)
  {
    app_destroy(app);
    return NULL;
  }

  return app;
}

void
app_destroy(GDTK_App* app)
{
  if (!app)
  {
    return;
  }

  LOG_INFO("Destroying GDTK_App");

  renderer_destroy(app->renderer);

  SDL_free(app);
  app = NULL;

  LOG_INFO("GDTK_App destroyed");
}
