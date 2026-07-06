#include "raylib.h"

enum screen {
  MENU,
  GAME
};

enum screen screen = MENU;

void drawmenu(const char *title, int currentWidth, int currentHeight) {
  Rectangle menuWindow = {currentWidth / 2.0f - 180,
                          currentHeight / 2.0f - 120, 360, 240};
  Rectangle playButton = {currentWidth / 2.0f - 70,
                          currentHeight / 2.0f + 30, 140, 50};
  Vector2 mouse = GetMousePosition();
  bool playHovered = CheckCollisionPointRec(mouse, playButton);

  if (playHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    screen = GAME;
  }

  ClearBackground(DARKGRAY);
  DrawRectangleRec(menuWindow, RAYWHITE);
  DrawRectangleLinesEx(menuWindow, 2, BLACK);

  DrawText(title, currentWidth / 2 - MeasureText(title, 40) / 2,
           currentHeight / 2 - 70, 40, BLACK);
  DrawRectangleRec(playButton, playHovered ? LIGHTGRAY : GRAY);
  DrawRectangleLinesEx(playButton, 2, BLACK);
  DrawText("PLAY", currentWidth / 2 - MeasureText("PLAY", 28) / 2,
           currentHeight / 2 + 42, 28, BLACK);
}

void drawgame(void) {
  ClearBackground(GREEN);
}

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;
  const char *title = "GAME";

  InitWindow(screenWidth, screenHeight, title);

  int monitor = GetCurrentMonitor();
  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  ToggleFullscreen();

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();

    BeginDrawing();

    switch (screen) {
    case MENU:
      drawmenu(title, currentWidth, currentHeight);
      break;

    case GAME:
      drawgame();
      break;
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
