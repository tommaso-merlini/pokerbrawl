#include "menu.h"

#include "menu_internal.h"

#include "../../game_state.h"
#include "../../ui.h"
#include "raylib.h"

#define MENU_TITLE "POKERBRAWL"

static MenuStep menuStep = MENU_STEP_MODE;

//++++++++++++STATE FUNCTIONS++++++++++++
static const char *stepTitle(MenuStep step) {
  switch (step) {
  case MENU_STEP_MODE:
    return "MODALITA";
  case MENU_STEP_CHARACTERS:
    return "PERSONAGGI";
  case MENU_STEP_MAPS:
    return "MAPPE";
  }

  return "";
}

static bool canAdvance(void) {
  if (menuStep == MENU_STEP_CHARACTERS) {
    return gGame.characterCount > 0;
  }
  if (menuStep == MENU_STEP_MAPS) {
    return gGame.selectedMapIndex >= 0 &&
           gGame.selectedMapIndex < gGame.availableMaps.count;
  }

  return true;
}

static void advanceMenu(void) {
  if (!canAdvance()) {
    return;
  }

  if (menuStep == MENU_STEP_MODE) {
    menuStep = MENU_STEP_CHARACTERS;
  } else if (menuStep == MENU_STEP_CHARACTERS) {
    menuStep = MENU_STEP_MAPS;
  } else if (menuStep == MENU_STEP_MAPS) {
    startGameWithMap(gGame.selectedMapIndex);
  }
}

static void previousMenu(void) {
  if (menuStep == MENU_STEP_CHARACTERS) {
    menuStep = MENU_STEP_MODE;
  } else if (menuStep == MENU_STEP_MAPS) {
    menuStep = MENU_STEP_CHARACTERS;
  }
}


//++++++++++++DRAW FUNCTIONS++++++++++++
static void drawHeader(MenuLayout layout) {
  Rectangle titleBounds = {layout.menuWindow.x + 24.0f,
                           layout.menuWindow.y + 24.0f,
                           layout.menuWindow.width - 48.0f, 44.0f};
  Rectangle stepsBounds = {layout.menuWindow.x + 36.0f,
                           layout.menuWindow.y + 74.0f,
                           layout.menuWindow.width - 72.0f, 26.0f};
  float stepWidth = stepsBounds.width / 3.0f;

  uiDrawCenteredText(MENU_TITLE, titleBounds, 40, 24, BLACK);

  for (int i = 0; i < 3; i++) {
    Rectangle step = {stepsBounds.x + (float)i * stepWidth, stepsBounds.y,
                      stepWidth, stepsBounds.height};
    bool selected = i == (int)menuStep;
    Color fill = selected ? BLUE : LIGHTGRAY;
    Color textColor = selected ? RAYWHITE : DARKGRAY;

    DrawRectangleRec(step, fill);
    DrawRectangleLinesEx(step, 1.0f, DARKGRAY);
    uiDrawCenteredText(stepTitle((MenuStep)i), step, 14, 10, textColor);
  }
}

static void drawFooter(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();
  bool nextEnabled = canAdvance();
  bool nextHovered =
      nextEnabled && CheckCollisionPointRec(mouse, layout.nextButton);
  const char *nextLabel = menuStep == MENU_STEP_MAPS ? "START" : "AVANTI";

  if (menuStep != MENU_STEP_MODE) {
    bool backHovered = CheckCollisionPointRec(mouse, layout.backButton);
    uiDrawCard("INDIETRO", layout.backButton, false, backHovered, true, 18);
  }

  uiDrawCard(nextLabel, layout.nextButton, false, nextHovered, nextEnabled, 18);
}


//++++++++++++MAIN FUNCTIONS++++++++++++
void menuScreenUpdate(void) {
  normalizeGameSelections();

  MenuLayout layout = menuGetLayout(GetScreenWidth(), GetScreenHeight());

  if (IsKeyPressed(KEY_ESCAPE)) {
    previousMenu();
  }
  if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
    advanceMenu();
  }

  if (menuStep != MENU_STEP_MODE && uiWasClicked(layout.backButton)) {
    previousMenu();
  }
  if (uiWasClicked(layout.nextButton)) {
    advanceMenu();
  }

  switch (menuStep) {
  case MENU_STEP_MODE:
    menuModeUpdate(layout);
    break;
  case MENU_STEP_CHARACTERS:
    menuCharactersUpdate(layout);
    break;
  case MENU_STEP_MAPS:
    menuMapsUpdate(layout);
    break;
  }
}

void menuScreenDraw(int currentWidth, int currentHeight) {
  normalizeGameSelections();

  MenuLayout layout = menuGetLayout(currentWidth, currentHeight);

  ClearBackground(DARKGRAY);
  DrawRectangleRec(layout.menuWindow, RAYWHITE);
  DrawRectangleLinesEx(layout.menuWindow, 2.0f, BLACK);

  drawHeader(layout);

  switch (menuStep) {
  case MENU_STEP_MODE:
    menuModeDraw(layout);
    break;
  case MENU_STEP_CHARACTERS:
    menuCharactersDraw(layout);
    break;
  case MENU_STEP_MAPS:
    menuMapsDraw(layout);
    break;
  }

  drawFooter(layout);
}
