//
// Created by Dwayne Edwards on 2026-06-20.
//

#include <stdlib.h>
#include <string.h>

#include <gdtk/gdtk.h>


#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define GAME_TITLE "GDTK Game"

typedef struct
{
  GDTK_App* app;
} Game;


GDTK_AppResult
gdtk_startup(void** gdtk_state, int argc, char* argv[])
{
  Game* game = (Game*)malloc(sizeof(Game));
  if (!game)
  {
    LOG_ERROR("Failed to allocate Game memory");
    return GDTK_APP_FAILURE;
  }
  memset(game, 0, sizeof(Game));

  (*gdtk_state) = game;

  const GDTK_Config cfg = {
      .title = GAME_TITLE,
      .dimensions =
          {
              .w = WINDOW_WIDTH,
              .h = WINDOW_HEIGHT,
          },
  };

  GDTK_App* app = app_create(&cfg);
  if (!app)
  {
    return GDTK_APP_FAILURE;
  }

  game->app = app;

  return GDTK_APP_CONTINUE;
}

GDTK_AppResult
gdtk_events(void* gdtk_state, const GDTK_Event* event)
{
  return GDTK_APP_CONTINUE;
}

GDTK_AppResult
gdtk_iterate(void* gdtk_state)
{
  return GDTK_APP_CONTINUE;
}

void
gdtk_shutdown(void* gdtk_state, const GDTK_AppResult result)
{
  Game* game = (Game*)gdtk_state;
  if (!game)
  {
    return;
  }

  app_destroy(game->app);

  free(game);
  game = NULL;
}
