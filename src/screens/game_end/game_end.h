#ifndef SCREENS_GAME_END_GAME_END_H
#define SCREENS_GAME_END_GAME_END_H

#include "../../game/game_state.h"
#include "../../input/keyboard.h"

void gameEndScreenUpdate(GameState *game, const KeyboardState *keyboard);
void gameEndScreenDraw(const GameState *game, int screenWidth,
                       int screenHeight);

#endif
