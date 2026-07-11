#include "map_renderer.h"

#include <stdio.h>
#include <string.h>

static void unloadBackground(MapRenderer *renderer) {
  if (IsTextureValid(renderer->background)) {
    UnloadTexture(renderer->background);
  }
  renderer->background = (Texture2D){0};
}

void initMapRenderer(MapRenderer *renderer) { *renderer = (MapRenderer){0}; }

void updateMapRenderer(MapRenderer *renderer, const ArenaMap *map) {
  const char *requestedPath =
      map->loaded && map->background[0] != '\0' ? map->background : "";
  if (strcmp(renderer->backgroundPath, requestedPath) == 0) {
    return;
  }

  unloadBackground(renderer);
  renderer->backgroundPath[0] = '\0';
  renderer->loadError[0] = '\0';
  if (requestedPath[0] == '\0') {
    return;
  }

  snprintf(renderer->backgroundPath, sizeof(renderer->backgroundPath), "%s",
           requestedPath);
  renderer->background = LoadTexture(requestedPath);
  if (!IsTextureValid(renderer->background)) {
    snprintf(renderer->loadError, sizeof(renderer->loadError),
             "Could not load map background %.460s", requestedPath);
    TraceLog(LOG_ERROR, "%s", renderer->loadError);
    return;
  }
  SetTextureFilter(renderer->background, TEXTURE_FILTER_POINT);
}

void unloadMapRenderer(MapRenderer *renderer) {
  unloadBackground(renderer);
  *renderer = (MapRenderer){0};
}
