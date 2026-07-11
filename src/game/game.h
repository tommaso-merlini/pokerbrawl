#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "../input/input.h"
#include "game_state.h"

void updateGame(GameState *game, const InputState *input, float dt);
void drawGame(const GameState *game, int screenWidth, int screenHeight);

#endif
