#include "menu_internal.h"

#include "../../ui/ui.h"

#include <stdio.h>

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

void menuDrawControllers(const ControllerRegistry *controllers,
                         MenuLayout layout) {
  const float rowHeight = 48.0f;
  const float padding = 14.0f;
  int rowCount = controllers->count > 0 ? controllers->count : 1;
  float height = 54.0f + (float)rowCount * rowHeight + padding;
  Rectangle panel = {
      layout.controllerRail.x + 12.0f,
      layout.controllerRail.y + (layout.controllerRail.height - height) * 0.5f,
      layout.controllerRail.width - 12.0f,
      height,
  };
  Rectangle header = {panel.x, panel.y, panel.width, 54.0f};

  DrawRectangleRec(panel, RAYWHITE);
  DrawRectangleRec(header, BLUE);
  DrawRectangleLinesEx(panel, 2.0f, BLACK);
  uiDrawCenteredText("CONTROLLER", header, 20, 14, RAYWHITE);

  if (controllers->count == 0) {
    Rectangle emptyRow = {panel.x + padding, header.y + header.height,
                          panel.width - padding * 2.0f, rowHeight};
    uiDrawCenteredText("Nessun controller", emptyRow, 16, 10, DARKGRAY);
    return;
  }

  for (int i = 0; i < controllers->count; i++) {
    Rectangle row = {panel.x + padding,
                     header.y + header.height + (float)i * rowHeight,
                     panel.width - padding * 2.0f, rowHeight};
    Rectangle indicator = {row.x, row.y + row.height * 0.5f - 5.0f, 10.0f,
                           10.0f};
    Rectangle label = {row.x + 18.0f, row.y, row.width - 18.0f, row.height};
    char text[CONTROLLER_NAME_CAPACITY + 16];

    snprintf(text, sizeof(text), "P%d  %s", i + 1,
             controllers->controllers[i].name);
    if (i > 0) {
      DrawLine((int)row.x, (int)row.y, (int)(row.x + row.width), (int)row.y,
               LIGHTGRAY);
    }
    DrawRectangleRec(indicator, GREEN);
    DrawRectangleLinesEx(indicator, 1.0f, DARKGREEN);
    uiDrawCenteredText(text, label, 16, 10, BLACK);
  }
}
