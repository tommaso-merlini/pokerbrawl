#ifndef SCREENS_GAME_GAME_SCREEN_H
#define SCREENS_GAME_GAME_SCREEN_H

#include "../../game/game_state.h"
#include "../../input/controller.h"
#include "../../input/keyboard.h"
#include "map_renderer.h"
#include "player_renderer.h"

void gameScreenUpdate(GameState *game, const KeyboardState *keyboard,
                      const ControllerRegistry *controllers, float dt);
// Pause menu (pause.c)
bool gamePauseUpdate(GameState *game, const KeyboardState *keyboard,
                     const ControllerRegistry *controllers);
void gamePauseDraw(const GameState *game, int screenWidth, int screenHeight);

void gameScreenDraw(const GameState *game, const PlayerRenderer *playerRenderer,
                    const MapRenderer *mapRenderer, int screenWidth,
                    int screenHeight);

#endif
