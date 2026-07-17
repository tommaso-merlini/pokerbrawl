#include "game/game.h"
#include "input/controller.h"
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
  MapRenderer mapRenderer;
  initMapRenderer(&mapRenderer);
  ControllerRegistry controllers;
  initControllerRegistry(&controllers);

  int monitor = GetCurrentMonitor();
  // SetWindowSize(1920 * 2, 1080 * 2);
  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  ToggleFullscreen();

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    updateControllerRegistry(&controllers);
    InputState input = readInput();
    float dt = GetFrameTime();
    updateGame(&game, &input, dt);
    updatePlayerRenderer(&playerRenderer, &game, dt);
    updateMapRenderer(&mapRenderer, &game.currentMap);

    BeginDrawing();
    drawGame(&game, &playerRenderer, &mapRenderer, GetScreenWidth(),
             GetScreenHeight());
    EndDrawing();
  }

  unloadMapRenderer(&mapRenderer);
  unloadPlayerRenderer(&playerRenderer);
  CloseWindow();

  return 0;
}
