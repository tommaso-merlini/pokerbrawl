#ifndef SCREENS_MENU_MENU_INTERNAL_H
#define SCREENS_MENU_MENU_INTERNAL_H

#include "../../assets/player_sprite_assets.h"
#include "../../game/game_state.h"
#include "../../input/controller.h"
#include "../../input/keyboard.h"
#include "../../ui/ui.h"
#include "raylib.h"

typedef struct MenuLayout {
  Rectangle menuWindow;
  Rectangle content;
  Rectangle backButton;
  Rectangle nextButton;
  Rectangle controllerRail;
} MenuLayout;

MenuLayout menuGetLayout(int currentWidth, int currentHeight);

Rectangle menuModeCard(Rectangle content);
int menuCharacterColumns(Rectangle content, int characterCount);
Rectangle menuCharacterCard(Rectangle content, int characterIndex,
                            int characterCount);
int menuMapColumns(Rectangle content);
Rectangle menuMapCard(Rectangle content, int mapIndex);

bool menuCanAdvance(const GameState *game);
void menuAdvance(GameState *game);
void menuPrevious(GameState *game);
void menuDrawFrame(const GameState *game, MenuLayout layout);
void menuDrawControllers(const ControllerRegistry *controllers,
                         MenuLayout layout);

void menuModeUpdate(GameState *game, const KeyboardState *keyboard,
                    MenuLayout layout);
void menuModeDraw(const GameState *game, MenuLayout layout);
void menuCharactersUpdate(GameState *game, const KeyboardState *keyboard,
                          MenuLayout layout);
void menuCharactersDraw(const GameState *game,
                        const PlayerSpriteAssets *spriteAssets,
                        MenuLayout layout);
void menuMapsUpdate(GameState *game, const KeyboardState *keyboard,
                    MenuLayout layout);
void menuMapsDraw(const GameState *game, MenuLayout layout);

#endif
