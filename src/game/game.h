#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "../input/controller.h"
#include "../input/keyboard.h"
#include "../screens/game/map_renderer.h"
#include "../screens/game/player_renderer.h"
#include "game_state.h"

void updateGame(GameState *game, const KeyboardState *keyboard,
                const ControllerRegistry *controllers, float dt);
void drawGame(const GameState *game, const PlayerRenderer *playerRenderer,
              const MapRenderer *mapRenderer,
              const ControllerRegistry *controllers, int screenWidth,
              int screenHeight);

#endif
