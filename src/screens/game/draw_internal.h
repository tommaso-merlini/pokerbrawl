#ifndef SCREENS_GAME_DRAW_INTERNAL_H
#define SCREENS_GAME_DRAW_INTERNAL_H

#include "game_screen.h"
#include "map_renderer.h"
#include "player_renderer.h"
#include "raylib.h"

Vector2 mapToScreen(Vector2 point, const ArenaMap *map, int width, int height);
void drawArena(const ArenaMap *map, const MapRenderer *renderer, int width,
               int height, bool showDebugBoxes);
void drawPlayers(const GameState *game, const PlayerRenderer *renderer,
                 int width, int height);
void drawGameLabels(const GameState *game);

#endif
