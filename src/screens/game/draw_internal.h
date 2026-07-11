#ifndef SCREENS_GAME_DRAW_INTERNAL_H
#define SCREENS_GAME_DRAW_INTERNAL_H

#include "game_screen.h"
#include "raylib.h"

Vector2 mapToScreen(Vector2 point, const ArenaMap *map, int width, int height);
void drawArena(const ArenaMap *map, int width, int height);
void drawPlayers(const GameState *game, int width, int height);
void drawGameLabels(const GameState *game);

#endif
