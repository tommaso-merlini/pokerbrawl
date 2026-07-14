#include "draw_internal.h"

#include <string.h>

#define SPAWNPOINT_SIZE 24.0f
#define DEBUG_LINE_WIDTH 3.0f

Vector2 mapToScreen(Vector2 point, const ArenaMap *map, int width, int height) {
  float scaleX = (float)width / (float)map->width;
  float scaleY = (float)height / (float)map->height;
  float scale = scaleX < scaleY ? scaleX : scaleY;
  float offsetX = ((float)width - (float)map->width * scale) * 0.5f;
  float offsetY = ((float)height - (float)map->height * scale) * 0.5f;
  return (Vector2){offsetX + point.x * scale, offsetY + point.y * scale};
}

static void drawQuad(const MapQuad *quad, const ArenaMap *map, int width,
                     int height) {
  Vector2 points[4];
  for (int i = 0; i < 4; i++) {
    points[i] = mapToScreen(quad->points[i], map, width, height);
  }

  Color fill = quad->collidable ? (Color){62, 140, 222, 230}
                                : (Color){190, 190, 190, 90};
  Color outline = quad->collidable ? (Color){20, 65, 130, 255}
                                   : (Color){105, 105, 105, 180};
  DrawTriangle(points[0], points[1], points[2], fill);
  DrawTriangle(points[0], points[2], points[3], fill);
  for (int i = 0; i < 4; i++) {
    DrawLineEx(points[i], points[(i + 1) % 4], 3.0f, outline);
  }
}

static void drawQuadHitbox(const MapQuad *quad, const ArenaMap *map, int width,
                           int height) {
  Vector2 points[4];
  for (int i = 0; i < 4; i++) {
    points[i] = mapToScreen(quad->points[i], map, width, height);
  }

  Color color = quad->collidable ? BLUE : GRAY;
  Color fill = color;
  fill.a = quad->collidable ? 80 : 45;
  DrawTriangle(points[0], points[1], points[2], fill);
  DrawTriangle(points[0], points[2], points[3], fill);
  for (int i = 0; i < 4; i++) {
    DrawLineEx(points[i], points[(i + 1) % 4], DEBUG_LINE_WIDTH, color);
  }
}

static void drawSpawnPoint(Vector2 spawn, const ArenaMap *map, int width,
                           int height) {
  float half = SPAWNPOINT_SIZE * 0.5f;
  Vector2 topLeft = mapToScreen((Vector2){spawn.x - half, spawn.y - half}, map,
                                width, height);
  Vector2 bottomRight = mapToScreen((Vector2){spawn.x + half, spawn.y + half},
                                    map, width, height);
  Rectangle bounds = {topLeft.x, topLeft.y, bottomRight.x - topLeft.x,
                      bottomRight.y - topLeft.y};
  DrawRectangleRec(bounds, GREEN);
}

static bool drawMapBackground(const ArenaMap *map, const MapRenderer *renderer,
                              int width, int height) {
  if (map->background[0] == '\0' ||
      strcmp(renderer->backgroundPath, map->background) != 0 ||
      !IsTextureValid(renderer->background)) {
    return false;
  }

  Vector2 topLeft = mapToScreen((Vector2){0.0f, 0.0f}, map, width, height);
  Vector2 bottomRight = mapToScreen(
      (Vector2){(float)map->width, (float)map->height}, map, width, height);
  Rectangle source = {0.0f, 0.0f, (float)renderer->background.width,
                      (float)renderer->background.height};
  Rectangle destination = {topLeft.x, topLeft.y, bottomRight.x - topLeft.x,
                           bottomRight.y - topLeft.y};
  DrawTexturePro(renderer->background, source, destination, (Vector2){0}, 0.0f,
                 WHITE);
  return true;
}

void drawArena(const ArenaMap *map, const MapRenderer *renderer, int width,
               int height) {
  bool hasBackground = drawMapBackground(map, renderer, width, height);

  if (!hasBackground) {
    for (int i = 0; i < map->quadCount; i++) {
      drawQuad(&map->quads[i], map, width, height);
    }
    for (int i = 0; i < map->spawnpointCount; i++) {
      drawSpawnPoint(map->spawnpoints[i], map, width, height);
    }
  }
}

void drawMapHitboxes(const ArenaMap *map, int width, int height) {
  for (int i = 0; i < map->quadCount; i++) {
    drawQuadHitbox(&map->quads[i], map, width, height);
  }

  for (int i = 0; i < map->spawnpointCount; i++) {
    drawSpawnPoint(map->spawnpoints[i], map, width, height);
  }
}
