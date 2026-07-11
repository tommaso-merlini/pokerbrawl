#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "../input/input.h"
#include "../screens/game/player_renderer.h"
#include "game_state.h"

void updateGame(GameState *game, const InputState *input, float dt);
void drawGame(const GameState *game, const PlayerRenderer *renderer,
              int screenWidth, int screenHeight);

#endif
