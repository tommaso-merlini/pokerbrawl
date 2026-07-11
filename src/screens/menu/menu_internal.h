#ifndef SCREENS_MENU_MENU_INTERNAL_H
#define SCREENS_MENU_MENU_INTERNAL_H

#include "raylib.h"

typedef enum MenuStep {
  MENU_STEP_MODE,
  MENU_STEP_CHARACTERS,
  MENU_STEP_MAPS
} MenuStep;

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
                            int characterIndex);
Rectangle menuMapCard(Rectangle content, int mapIndex);

void menuModeUpdate(MenuLayout layout);
void menuModeDraw(MenuLayout layout);
void menuCharactersUpdate(MenuLayout layout);
void menuCharactersDraw(MenuLayout layout);
void menuMapsUpdate(MenuLayout layout);
void menuMapsDraw(MenuLayout layout);

#endif
