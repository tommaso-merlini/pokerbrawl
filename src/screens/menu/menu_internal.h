#ifndef SCREENS_MENU_MENU_INTERNAL_H
#define SCREENS_MENU_MENU_INTERNAL_H

#include "../../game/game_state.h"
#include "../../input/input.h"
#include "../../ui/ui.h"
#include "raylib.h"

typedef struct MenuLayout {
  Rectangle menuWindow;
  Rectangle content;
  Rectangle backButton;
  Rectangle nextButton;
} MenuLayout;

MenuLayout menuGetLayout(int currentWidth, int currentHeight);

Rectangle menuModeCard(Rectangle content);
Rectangle menuPlayerSection(Rectangle content, int playerIndex);
Rectangle menuCharacterCard(Rectangle content, int playerIndex,
                            int characterIndex, int characterCount);
Rectangle menuMapCard(Rectangle content, int mapIndex);

bool menuCanAdvance(const GameState *game);
void menuAdvance(GameState *game);
void menuPrevious(GameState *game);
void menuDrawFrame(const GameState *game, MenuLayout layout);

void menuModeUpdate(GameState *game, const InputState *input,
                    MenuLayout layout);
void menuModeDraw(const GameState *game, MenuLayout layout);
void menuCharactersUpdate(GameState *game, const InputState *input,
                          MenuLayout layout);
void menuCharactersDraw(const GameState *game, MenuLayout layout);
void menuMapsUpdate(GameState *game, const InputState *input,
                    MenuLayout layout);
void menuMapsDraw(const GameState *game, MenuLayout layout);

#endif
