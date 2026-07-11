#include "menu_internal.h"

#include "../../ui/ui.h"

#define MENU_TITLE "POKERBRAWL"

static const char *stepTitle(MenuStep step) {
  static const char *TITLES[] = {"MODALITA", "PERSONAGGI", "MAPPE"};
  return TITLES[step];
}

static void drawHeader(const GameState *game, MenuLayout layout) {
  Rectangle title = {layout.menuWindow.x + 24.0f, layout.menuWindow.y + 24.0f,
                     layout.menuWindow.width - 48.0f, 44.0f};
  Rectangle steps = {layout.menuWindow.x + 36.0f, layout.menuWindow.y + 74.0f,
                     layout.menuWindow.width - 72.0f, 26.0f};
  float width = steps.width / 3.0f;

  uiDrawCenteredText(MENU_TITLE, title, 40, 24, BLACK);
  for (int i = 0; i < 3; i++) {
    Rectangle bounds = {steps.x + (float)i * width, steps.y, width,
                        steps.height};
    bool selected = i == (int)game->menu.step;
    DrawRectangleRec(bounds, selected ? BLUE : LIGHTGRAY);
    DrawRectangleLinesEx(bounds, 1.0f, DARKGRAY);
    uiDrawCenteredText(stepTitle((MenuStep)i), bounds, 14, 10,
                       selected ? RAYWHITE : DARKGRAY);
  }
}

static void drawFooter(const GameState *game, MenuLayout layout) {
  bool enabled = menuCanAdvance(game);
  bool nextHovered =
      enabled && CheckCollisionPointRec(game->ui.pointer, layout.nextButton);
  const char *next = game->menu.step == MENU_STEP_MAPS ? "START" : "AVANTI";

  if (game->menu.step != MENU_STEP_MODE) {
    bool hovered = CheckCollisionPointRec(game->ui.pointer, layout.backButton);
    uiDrawCard("INDIETRO", layout.backButton, false, hovered, true, 18);
  }
  uiDrawCard(next, layout.nextButton, false, nextHovered, enabled, 18);
}

void menuDrawFrame(const GameState *game, MenuLayout layout) {
  ClearBackground(DARKGRAY);
  DrawRectangleRec(layout.menuWindow, RAYWHITE);
  DrawRectangleLinesEx(layout.menuWindow, 2.0f, BLACK);
  drawHeader(game, layout);
  drawFooter(game, layout);
}
