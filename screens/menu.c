#include "menu.h"

#include "../game_state.h"
#include "raylib.h"

#define MENU_TITLE "BRAWLHALLA"

typedef struct MenuLayout {
  Rectangle menuWindow;
  Rectangle mapList;
  float buttonHeight;
  float buttonGap;
} MenuLayout;

static float menuScroll = 0.0f;

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
  const float panelWidth = 480.0f;
  float panelHeight = currentHeight - 100.0f;

  if (panelHeight > 560.0f) {
    panelHeight = 560.0f;
  }
  if (panelHeight < 260.0f) {
    panelHeight = currentHeight - 40.0f;
  }
  if (panelHeight < 180.0f) {
    panelHeight = 180.0f;
  }

  MenuLayout layout = {
      .menuWindow = {currentWidth / 2.0f - panelWidth * 0.5f,
                     currentHeight / 2.0f - panelHeight * 0.5f, panelWidth,
                     panelHeight},
      .buttonHeight = 48.0f,
      .buttonGap = 12.0f,
  };

  layout.mapList =
      (Rectangle){layout.menuWindow.x + 50.0f, layout.menuWindow.y + 126.0f,
                  layout.menuWindow.width - 100.0f,
                  layout.menuWindow.height - 166.0f};

  return layout;
}

static float getmaxscroll(MenuLayout layout) {
  int mapCount = gGame.availableMaps.count;
  float contentHeight =
      mapCount * layout.buttonHeight + (mapCount - 1) * layout.buttonGap;

  return contentHeight > layout.mapList.height
             ? contentHeight - layout.mapList.height
             : 0.0f;
}

static void clampmenuscroll(float maxScroll) {
  if (menuScroll < 0.0f || maxScroll <= 0.0f) {
    menuScroll = 0.0f;
  }
  if (menuScroll > maxScroll) {
    menuScroll = maxScroll;
  }
}

void menuScreenUpdate(void) {
  int mapCount = gGame.availableMaps.count;

  if (mapCount <= 0) {
    return;
  }

  MenuLayout layout = getmenulayout(GetScreenWidth(), GetScreenHeight());
  Vector2 mouse = GetMousePosition();
  float maxScroll = getmaxscroll(layout);

  if (CheckCollisionPointRec(mouse, layout.mapList)) {
    menuScroll -= GetMouseWheelMove() * 36.0f;
  }
  clampmenuscroll(maxScroll);

  for (int i = 0; i < mapCount; i++) {
    Rectangle mapButton = {
        layout.mapList.x,
        layout.mapList.y + i * (layout.buttonHeight + layout.buttonGap) -
            menuScroll,
        layout.mapList.width, layout.buttonHeight};
    bool visible =
        mapButton.y + mapButton.height >= layout.mapList.y &&
        mapButton.y <= layout.mapList.y + layout.mapList.height;
    bool hovered = visible && CheckCollisionPointRec(mouse, layout.mapList) &&
                   CheckCollisionPointRec(mouse, mapButton);

    if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      startGameWithMap(i);
      return;
    }
  }
}

void menuScreenDraw(int currentWidth, int currentHeight) {
  MenuLayout layout = getmenulayout(currentWidth, currentHeight);
  int mapCount = gGame.availableMaps.count;
  Vector2 mouse = GetMousePosition();

  ClearBackground(DARKGRAY);
  DrawRectangleRec(layout.menuWindow, RAYWHITE);
  DrawRectangleLinesEx(layout.menuWindow, 2, BLACK);

  DrawText(MENU_TITLE, currentWidth / 2 - MeasureText(MENU_TITLE, 40) / 2,
           (int)layout.menuWindow.y + 32, 40, BLACK);
  DrawText("MAPPE", currentWidth / 2 - MeasureText("MAPPE", 20) / 2,
           (int)layout.menuWindow.y + 84, 20, DARKGRAY);

  if (mapCount <= 0) {
    Rectangle messageBounds = {layout.mapList.x, layout.mapList.y,
                               layout.mapList.width, 60.0f};

    drawcenteredtext(gGame.mapLoadError[0] != '\0' ? gGame.mapLoadError
                                                   : "Nessuna mappa trovata",
                     messageBounds, 20, 12, RED);
    return;
  }

  float maxScroll = getmaxscroll(layout);
  clampmenuscroll(maxScroll);

  BeginScissorMode((int)layout.mapList.x, (int)layout.mapList.y,
                   (int)layout.mapList.width, (int)layout.mapList.height);

  for (int i = 0; i < mapCount; i++) {
    Rectangle mapButton = {
        layout.mapList.x,
        layout.mapList.y + i * (layout.buttonHeight + layout.buttonGap) -
            menuScroll,
        layout.mapList.width, layout.buttonHeight};
    bool visible =
        mapButton.y + mapButton.height >= layout.mapList.y &&
        mapButton.y <= layout.mapList.y + layout.mapList.height;
    bool hovered = visible && CheckCollisionPointRec(mouse, layout.mapList) &&
                   CheckCollisionPointRec(mouse, mapButton);

    if (!visible) {
      continue;
    }

    DrawRectangleRec(mapButton, hovered ? LIGHTGRAY : GRAY);
    DrawRectangleLinesEx(mapButton, 2, BLACK);
    drawcenteredtext(gGame.availableMaps.maps[i].name, mapButton, 24, 12,
                     BLACK);
  }

  EndScissorMode();

  if (maxScroll > 0.0f) {
    float contentHeight = mapCount * layout.buttonHeight +
                          (mapCount - 1) * layout.buttonGap;
    Rectangle scrollTrack = {layout.mapList.x + layout.mapList.width + 12.0f,
                             layout.mapList.y, 6.0f,
                             layout.mapList.height};
    float thumbHeight =
        layout.mapList.height * layout.mapList.height / contentHeight;

    if (thumbHeight < 24.0f) {
      thumbHeight = 24.0f;
    }

    Rectangle scrollThumb = {
        scrollTrack.x,
        scrollTrack.y + (menuScroll / maxScroll) *
                            (scrollTrack.height - thumbHeight),
        scrollTrack.width, thumbHeight};

    DrawRectangleRec(scrollTrack, LIGHTGRAY);
    DrawRectangleRec(scrollThumb, DARKGRAY);
  }
}
