#ifndef SCREENS_GAME_GAME_SCREEN_H
#define SCREENS_GAME_GAME_SCREEN_H

#include "../../game/game_state.h"
#include "../../input/input.h"
#include "player_renderer.h"

void gameScreenUpdate(GameState *game, const InputState *input, float dt);
void gameScreenDraw(const GameState *game, const PlayerRenderer *renderer,
                    int screenWidth, int screenHeight);

#endif
