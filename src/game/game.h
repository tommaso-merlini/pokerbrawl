#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "../input/input.h"
#include "../screens/game/map_renderer.h"
#include "../screens/game/player_renderer.h"
#include "game_state.h"

void updateGame(GameState *game, const InputState *input, float dt);
void drawGame(const GameState *game, const PlayerRenderer *playerRenderer,
              const MapRenderer *mapRenderer, int screenWidth,
              int screenHeight);

#endif
