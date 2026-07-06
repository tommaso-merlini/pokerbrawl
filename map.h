#ifndef MAP_H
#define MAP_H

#include "raylib.h"

#define MAX_MAPS 16
#define MAX_MAP_NAME 64
#define MAX_QUAD_ID 64
#define MAX_MAP_QUADS 128
#define MAX_MAP_SPAWNPOINTS 16

typedef struct MapQuad {
  char id[MAX_QUAD_ID];
  Vector2 points[4];
  bool collidable;
} MapQuad;

typedef struct ArenaMap {
  char name[MAX_MAP_NAME];
  int width;
  int height;
  MapQuad quads[MAX_MAP_QUADS];
  int quadCount;
  Vector2 spawnpoints[MAX_MAP_SPAWNPOINTS];
  int spawnpointCount;
  bool loaded;
} ArenaMap;

typedef struct MapList {
  ArenaMap maps[MAX_MAPS];
  int count;
} MapList;

#endif
