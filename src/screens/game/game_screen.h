#ifndef SCREENS_GAME_GAME_SCREEN_H
#define SCREENS_GAME_GAME_SCREEN_H

#include "../../game/game_state.h"
#include "../../input/controller.h"
#include "../../input/input.h"
#include "map_renderer.h"
#include "player_renderer.h"

void gameScreenUpdate(GameState *game, const InputState *input,
                      const ControllerRegistry *controllers, float dt);
void gameScreenDraw(const GameState *game, const PlayerRenderer *playerRenderer,
                    const MapRenderer *mapRenderer, int screenWidth,
                    int screenHeight);

#endif
