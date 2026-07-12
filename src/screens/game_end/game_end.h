#ifndef SCREENS_GAME_END_GAME_END_H
#define SCREENS_GAME_END_GAME_END_H

#include "../../game/game_state.h"
#include "../../input/input.h"

void gameEndScreenUpdate(GameState *game, const InputState *input);
void gameEndScreenDraw(const GameState *game, int screenWidth,
                       int screenHeight);

#endif
