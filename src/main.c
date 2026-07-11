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
  PlayerRenderer playerRenderer;
  if (!initPlayerRenderer(&playerRenderer)) {
    TraceLog(LOG_ERROR, "%s", playerRenderer.loadError);
  }

  int monitor = GetCurrentMonitor();
  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  ToggleFullscreen();

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    InputState input = readInput();
    float dt = GetFrameTime();
    updateGame(&game, &input, dt);
    updatePlayerRenderer(&playerRenderer, &game, dt);

    BeginDrawing();
    drawGame(&game, &playerRenderer, GetScreenWidth(), GetScreenHeight());
    EndDrawing();
  }

  unloadPlayerRenderer(&playerRenderer);
  CloseWindow();

  return 0;
}
