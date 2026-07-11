#ifndef SCREENS_GAME_MAP_RENDERER_H
#define SCREENS_GAME_MAP_RENDERER_H

#include "../../maps/map.h"
#include "raylib.h"

typedef struct MapRenderer {
  Texture2D background;
  char backgroundPath[MAX_MAP_BACKGROUND];
  char loadError[512];
} MapRenderer;

void initMapRenderer(MapRenderer *renderer);
void updateMapRenderer(MapRenderer *renderer, const ArenaMap *map);
void unloadMapRenderer(MapRenderer *renderer);

#endif
