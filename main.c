#include "raylib.h"
#include "game_state.h"
#include "screens/game.h"
#include "screens/menu.h"

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;
  const char *title = "BRAWLHALLA";

  InitWindow(screenWidth, screenHeight, title);
  initGameState();

  int monitor = GetCurrentMonitor();
  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  ToggleFullscreen();

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();

    switch (gGame.screen) {
    case SCREEN_MENU:
      menuScreenUpdate();
      break;

    case SCREEN_GAME:
      gameScreenUpdate();
      break;
    }

    BeginDrawing();

    switch (gGame.screen) {
    case SCREEN_MENU:
      menuScreenDraw(currentWidth, currentHeight);
      break;

    case SCREEN_GAME:
      gameScreenDraw(currentWidth, currentHeight);
      break;
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
