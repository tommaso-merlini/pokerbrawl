#include "menu.h"

#include "../game_state.h"
#include "raylib.h"

#define MENU_TITLE "BRAWLHALLA"

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

static MenuStep menuStep = MENU_STEP_MODE;
static float mapScroll = 0.0f;

static int fittextsize(const char *text, int maxWidth, int fontSize,
                       int minFontSize) {
  while (fontSize > minFontSize && MeasureText(text, fontSize) > maxWidth) {
    fontSize -= 2;
  }

  return fontSize;
}

static void drawcenteredtext(const char *text, Rectangle bounds, int fontSize,
                             int minFontSize, Color color) {
  int size = fittextsize(text, (int)bounds.width - 24, fontSize, minFontSize);
  int textWidth = MeasureText(text, size);

  DrawText(text, (int)(bounds.x + bounds.width * 0.5f - textWidth * 0.5f),
           (int)(bounds.y + bounds.height * 0.5f - size * 0.5f), size, color);
}

static MenuLayout getmenulayout(int currentWidth, int currentHeight) {
  float panelWidth = (float)currentWidth - 80.0f;
  float panelHeight = (float)currentHeight - 90.0f;

  if (panelWidth > 900.0f) {
    panelWidth = 900.0f;
  }
  if (panelWidth < 520.0f) {
    panelWidth = (float)currentWidth - 32.0f;
  }
  if (panelWidth < 300.0f) {
    panelWidth = 300.0f;
  }

  if (panelHeight > 680.0f) {
    panelHeight = 680.0f;
  }
  if (panelHeight < 340.0f) {
    panelHeight = (float)currentHeight - 32.0f;
  }
  if (panelHeight < 260.0f) {
    panelHeight = 260.0f;
  }

  MenuLayout layout = {0};
  layout.menuWindow =
      (Rectangle){(float)currentWidth * 0.5f - panelWidth * 0.5f,
                  (float)currentHeight * 0.5f - panelHeight * 0.5f,
                  panelWidth, panelHeight};

  float buttonWidth = (layout.menuWindow.width - 104.0f) * 0.5f;
  if (buttonWidth > 150.0f) {
    buttonWidth = 150.0f;
  }
  if (buttonWidth < 104.0f) {
    buttonWidth = 104.0f;
  }

  layout.content =
      (Rectangle){layout.menuWindow.x + 36.0f, layout.menuWindow.y + 112.0f,
                  layout.menuWindow.width - 72.0f,
                  layout.menuWindow.height - 194.0f};
  if (layout.content.height < 120.0f) {
    layout.content.height = 120.0f;
  }

  layout.backButton =
      (Rectangle){layout.menuWindow.x + 36.0f,
                  layout.menuWindow.y + layout.menuWindow.height - 58.0f,
                  buttonWidth, 42.0f};
  layout.nextButton =
      (Rectangle){layout.menuWindow.x + layout.menuWindow.width - 36.0f -
                      buttonWidth,
                  layout.backButton.y, buttonWidth, layout.backButton.height};

  return layout;
}

static const char *steptitle(MenuStep step) {
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

static const char *modelabel(GameMode mode) {
  switch (mode) {
  case GAME_MODE_1V1:
    return "1 VS 1";
  }

  return "";
}

static void drawcard(const char *text, Rectangle bounds, bool selected,
                     bool hovered, bool enabled, int fontSize) {
  Color fill = enabled ? (Color){210, 210, 210, 255}
                       : (Color){170, 170, 170, 255};
  Color outline = enabled ? BLACK : DARKGRAY;
  Color textColor = enabled ? BLACK : DARKGRAY;

  if (selected && enabled) {
    fill = (Color){35, 115, 230, 255};
    outline = (Color){12, 45, 120, 255};
    textColor = RAYWHITE;
  } else if (hovered && enabled) {
    fill = (Color){235, 235, 235, 255};
  }

  DrawRectangleRec(bounds, fill);
  DrawRectangleLinesEx(bounds, 2.0f, outline);
  drawcenteredtext(text, bounds, fontSize, 12, textColor);
}

static bool wasclicked(Rectangle bounds) {
  return CheckCollisionPointRec(GetMousePosition(), bounds) &&
         IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

static bool canadvance(void) {
  if (menuStep == MENU_STEP_CHARACTERS) {
    return gGame.characterCount > 0;
  }
  if (menuStep == MENU_STEP_MAPS) {
    return gGame.selectedMapIndex >= 0 &&
           gGame.selectedMapIndex < gGame.availableMaps.count;
  }

  return true;
}

static void normalizemenuselection(void) {
  if (gGame.selectedMode != GAME_MODE_1V1) {
    gGame.selectedMode = GAME_MODE_1V1;
  }

  if (gGame.characterCount > 0) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
      int selectedCharacter = gGame.playerSetups[i].selectedCharacterIndex;
      if (selectedCharacter < 0 || selectedCharacter >= gGame.characterCount) {
        gGame.playerSetups[i].selectedCharacterIndex = 0;
      }
    }
  }

  if (gGame.availableMaps.count > 0 &&
      (gGame.selectedMapIndex < 0 ||
       gGame.selectedMapIndex >= gGame.availableMaps.count)) {
    gGame.selectedMapIndex = 0;
  }
}

static void advancemenu(void) {
  if (!canadvance()) {
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

static void previousmenu(void) {
  if (menuStep == MENU_STEP_CHARACTERS) {
    menuStep = MENU_STEP_MODE;
  } else if (menuStep == MENU_STEP_MAPS) {
    menuStep = MENU_STEP_CHARACTERS;
  }
}

static Rectangle modecard(Rectangle content) {
  float cardWidth = content.width;
  float cardHeight = 82.0f;

  if (cardWidth > 360.0f) {
    cardWidth = 360.0f;
  }
  if (content.height < cardHeight) {
    cardHeight = content.height;
  }

  return (Rectangle){content.x + content.width * 0.5f - cardWidth * 0.5f,
                     content.y + content.height * 0.5f - cardHeight * 0.5f,
                     cardWidth, cardHeight};
}

static Rectangle playersection(Rectangle content, int playerIndex) {
  float gap = 18.0f;
  float width = (content.width - gap) * 0.5f;

  return (Rectangle){content.x + (float)playerIndex * (width + gap),
                     content.y, width, content.height};
}

static Rectangle charactergrid(Rectangle section) {
  return (Rectangle){section.x, section.y + 34.0f, section.width,
                     section.height - 34.0f};
}

static int charactercolumns(Rectangle section) {
  return section.width >= 230.0f ? 2 : 1;
}

static Rectangle charactercard(Rectangle content, int playerIndex,
                               int characterIndex) {
  Rectangle section = playersection(content, playerIndex);
  Rectangle grid = charactergrid(section);
  int columns = charactercolumns(section);
  int rows = (gGame.characterCount + columns - 1) / columns;
  float gap = 10.0f;
  float cardHeight =
      rows > 0 ? (grid.height - (float)(rows - 1) * gap) / (float)rows : 44.0f;

  if (cardHeight > 56.0f) {
    cardHeight = 56.0f;
  }
  if (cardHeight < 28.0f) {
    cardHeight = 28.0f;
  }

  float cardWidth = (grid.width - (float)(columns - 1) * gap) / (float)columns;
  int row = characterIndex / columns;
  int column = characterIndex % columns;

  return (Rectangle){grid.x + (float)column * (cardWidth + gap),
                     grid.y + (float)row * (cardHeight + gap), cardWidth,
                     cardHeight};
}

static int mapcolumns(Rectangle content) {
  if (content.width >= 760.0f) {
    return 4;
  }
  if (content.width >= 560.0f) {
    return 3;
  }

  return 2;
}

static float mapcardheight(Rectangle content) {
  return content.height < 190.0f ? 58.0f : 74.0f;
}

static float mapcontentheight(Rectangle content) {
  int mapCount = gGame.availableMaps.count;
  int columns = mapcolumns(content);
  int rows = (mapCount + columns - 1) / columns;
  float cardHeight = mapcardheight(content);
  float gap = 12.0f;

  if (rows <= 0) {
    return 0.0f;
  }

  return (float)rows * cardHeight + (float)(rows - 1) * gap;
}

static float maxmapscroll(Rectangle content) {
  float height = mapcontentheight(content);
  return height > content.height ? height - content.height : 0.0f;
}

static void clampmapscroll(Rectangle content) {
  float maxScroll = maxmapscroll(content);

  if (mapScroll < 0.0f || maxScroll <= 0.0f) {
    mapScroll = 0.0f;
  }
  if (mapScroll > maxScroll) {
    mapScroll = maxScroll;
  }
}

static Rectangle mapcard(Rectangle content, int mapIndex) {
  int columns = mapcolumns(content);
  float gap = 12.0f;
  float cardHeight = mapcardheight(content);
  float cardWidth = (content.width - (float)(columns - 1) * gap) /
                    (float)columns;
  int row = mapIndex / columns;
  int column = mapIndex % columns;

  return (Rectangle){content.x + (float)column * (cardWidth + gap),
                     content.y + (float)row * (cardHeight + gap) - mapScroll,
                     cardWidth, cardHeight};
}

static void updatemodeslide(MenuLayout layout) {
  Rectangle card = modecard(layout.content);

  if (wasclicked(card)) {
    gGame.selectedMode = GAME_MODE_1V1;
  }
}

static void updatecharactersslide(MenuLayout layout) {
  if (gGame.characterCount <= 0) {
    return;
  }

  for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
    for (int i = 0; i < gGame.characterCount; i++) {
      Rectangle card = charactercard(layout.content, playerIndex, i);
      if (wasclicked(card)) {
        gGame.playerSetups[playerIndex].selectedCharacterIndex = i;
      }
    }
  }
}

static void updatemapsslide(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();

  if (CheckCollisionPointRec(mouse, layout.content)) {
    mapScroll -= GetMouseWheelMove() * 40.0f;
  }
  clampmapscroll(layout.content);

  for (int i = 0; i < gGame.availableMaps.count; i++) {
    Rectangle card = mapcard(layout.content, i);
    bool visible = card.y + card.height >= layout.content.y &&
                   card.y <= layout.content.y + layout.content.height;

    if (visible && CheckCollisionPointRec(mouse, layout.content) &&
        wasclicked(card)) {
      gGame.selectedMapIndex = i;
    }
  }
}

static void drawheader(MenuLayout layout) {
  Rectangle titleBounds = {layout.menuWindow.x + 24.0f,
                           layout.menuWindow.y + 24.0f,
                           layout.menuWindow.width - 48.0f, 44.0f};
  Rectangle stepsBounds = {layout.menuWindow.x + 36.0f,
                           layout.menuWindow.y + 74.0f,
                           layout.menuWindow.width - 72.0f, 26.0f};
  float stepWidth = stepsBounds.width / 3.0f;

  drawcenteredtext(MENU_TITLE, titleBounds, 40, 24, BLACK);

  for (int i = 0; i < 3; i++) {
    Rectangle step = {stepsBounds.x + (float)i * stepWidth, stepsBounds.y,
                      stepWidth, stepsBounds.height};
    bool selected = i == (int)menuStep;
    Color fill = selected ? (Color){35, 115, 230, 255}
                          : (Color){210, 210, 210, 255};
    Color textColor = selected ? RAYWHITE : DARKGRAY;

    DrawRectangleRec(step, fill);
    DrawRectangleLinesEx(step, 1.0f, selected ? (Color){12, 45, 120, 255}
                                             : DARKGRAY);
    drawcenteredtext(steptitle((MenuStep)i), step, 14, 10, textColor);
  }
}

static void drawfooter(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();
  bool nextEnabled = canadvance();
  bool nextHovered =
      nextEnabled && CheckCollisionPointRec(mouse, layout.nextButton);
  const char *nextLabel = menuStep == MENU_STEP_MAPS ? "START" : "AVANTI";

  if (menuStep != MENU_STEP_MODE) {
    bool backHovered = CheckCollisionPointRec(mouse, layout.backButton);
    drawcard("INDIETRO", layout.backButton, false, backHovered, true, 18);
  }

  drawcard(nextLabel, layout.nextButton, false, nextHovered, nextEnabled, 18);
}

static void drawmodeslide(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();
  Rectangle card = modecard(layout.content);
  bool hovered = CheckCollisionPointRec(mouse, card);

  drawcard(modelabel(gGame.selectedMode), card, true, hovered, true, 26);
}

static void drawcharactersslide(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();

  if (gGame.characterCount <= 0) {
    drawcenteredtext("Nessun personaggio trovato", layout.content, 22, 12, RED);
    return;
  }

  for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
    Rectangle section = playersection(layout.content, playerIndex);
    const char *label = playerIndex == 0 ? "Player 1" : "Player 2";
    int selectedCharacter =
        gGame.playerSetups[playerIndex].selectedCharacterIndex;

    DrawText(label, (int)section.x, (int)section.y, 24, BLACK);

    for (int i = 0; i < gGame.characterCount; i++) {
      Rectangle card = charactercard(layout.content, playerIndex, i);
      bool selected = i == selectedCharacter;
      bool hovered = CheckCollisionPointRec(mouse, card);

      drawcard(gGame.availableCharacters[i].name, card, selected, hovered, true,
               20);
    }
  }
}

static void drawmapsslide(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();

  if (gGame.availableMaps.count <= 0) {
    drawcenteredtext(gGame.mapLoadError[0] != '\0' ? gGame.mapLoadError
                                                   : "Nessuna mappa trovata",
                     layout.content, 22, 12, RED);
    return;
  }

  clampmapscroll(layout.content);

  BeginScissorMode((int)layout.content.x, (int)layout.content.y,
                   (int)layout.content.width, (int)layout.content.height);

  for (int i = 0; i < gGame.availableMaps.count; i++) {
    Rectangle card = mapcard(layout.content, i);
    bool visible = card.y + card.height >= layout.content.y &&
                   card.y <= layout.content.y + layout.content.height;
    bool selected = i == gGame.selectedMapIndex;
    bool hovered = visible && CheckCollisionPointRec(mouse, layout.content) &&
                   CheckCollisionPointRec(mouse, card);

    if (!visible) {
      continue;
    }

    drawcard(gGame.availableMaps.maps[i].name, card, selected, hovered, true,
             20);
  }

  EndScissorMode();

  if (maxmapscroll(layout.content) > 0.0f) {
    float contentHeight = mapcontentheight(layout.content);
    float maxScroll = maxmapscroll(layout.content);
    Rectangle scrollTrack = {layout.content.x + layout.content.width + 10.0f,
                             layout.content.y, 6.0f, layout.content.height};
    float thumbHeight =
        layout.content.height * layout.content.height / contentHeight;

    if (thumbHeight < 24.0f) {
      thumbHeight = 24.0f;
    }

    Rectangle scrollThumb = {
        scrollTrack.x,
        scrollTrack.y + (mapScroll / maxScroll) *
                            (scrollTrack.height - thumbHeight),
        scrollTrack.width, thumbHeight};

    DrawRectangleRec(scrollTrack, LIGHTGRAY);
    DrawRectangleRec(scrollThumb, DARKGRAY);
  }
}

void menuScreenUpdate(void) {
  normalizemenuselection();

  MenuLayout layout = getmenulayout(GetScreenWidth(), GetScreenHeight());

  if (IsKeyPressed(KEY_ESCAPE)) {
    previousmenu();
  }
  if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
    advancemenu();
  }

  if (menuStep != MENU_STEP_MODE && wasclicked(layout.backButton)) {
    previousmenu();
  }
  if (wasclicked(layout.nextButton)) {
    advancemenu();
  }

  switch (menuStep) {
  case MENU_STEP_MODE:
    updatemodeslide(layout);
    break;
  case MENU_STEP_CHARACTERS:
    updatecharactersslide(layout);
    break;
  case MENU_STEP_MAPS:
    updatemapsslide(layout);
    break;
  }
}

void menuScreenDraw(int currentWidth, int currentHeight) {
  normalizemenuselection();

  MenuLayout layout = getmenulayout(currentWidth, currentHeight);

  ClearBackground(DARKGRAY);
  DrawRectangleRec(layout.menuWindow, RAYWHITE);
  DrawRectangleLinesEx(layout.menuWindow, 2.0f, BLACK);

  drawheader(layout);

  switch (menuStep) {
  case MENU_STEP_MODE:
    drawmodeslide(layout);
    break;
  case MENU_STEP_CHARACTERS:
    drawcharactersslide(layout);
    break;
  case MENU_STEP_MAPS:
    drawmapsslide(layout);
    break;
  }

  drawfooter(layout);
}
