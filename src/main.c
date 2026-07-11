#include "game/game.h"
#include "input/input.h"
#include "raylib.h"

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;
  const char *title = "PokerBrawl";

  InitWindow(screenWidth, screenHeight, title);
  GameState game;
  initGameState(&game);

  int monitor = GetCurrentMonitor();
  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  ToggleFullscreen();

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    InputState input = readInput();
    updateGame(&game, &input, GetFrameTime());

    BeginDrawing();
    drawGame(&game, GetScreenWidth(), GetScreenHeight());
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
