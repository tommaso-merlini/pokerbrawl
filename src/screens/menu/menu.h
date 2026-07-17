#ifndef SCREENS_MENU_MENU_H
#define SCREENS_MENU_MENU_H

#include "../../game/game_state.h"
#include "../../input/controller.h"
#include "../../input/input.h"

void menuScreenUpdate(GameState *game, const InputState *input);
void menuScreenDraw(const GameState *game,
                    const ControllerRegistry *controllers, int screenWidth,
                    int screenHeight);

#endif
