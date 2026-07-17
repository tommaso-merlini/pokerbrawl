#ifndef SCREENS_MENU_MENU_H
#define SCREENS_MENU_MENU_H

#include "../../assets/player_sprite_assets.h"
#include "../../game/game_state.h"
#include "../../input/controller.h"
#include "../../input/input.h"
#include "../../ui/navigation.h"

UiNavigationTarget menuScreenNavigation(GameState *game);
void menuScreenUpdate(GameState *game, const InputState *input,
                      const ControllerRegistry *controllers);
void menuScreenDraw(const GameState *game,
                    const PlayerSpriteAssets *spriteAssets,
                    const ControllerRegistry *controllers, int screenWidth,
                    int screenHeight);

#endif
