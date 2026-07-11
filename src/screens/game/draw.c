#include "draw_internal.h"

#include <stdio.h>

void gameScreenDraw(const GameState *game, const PlayerRenderer *playerRenderer,
                    const MapRenderer *mapRenderer, int screenWidth,
                    int screenHeight) {
  ClearBackground(DARKBROWN);

  if (!game->currentMap.loaded) {
    const char *error =
        game->mapLoadError[0] != '\0' ? game->mapLoadError : "No map loaded";
    DrawText(error, 30, 30, 22, RED);
    return;
  }

  drawArena(&game->currentMap, mapRenderer, screenWidth, screenHeight);
  drawPlayers(game, playerRenderer, screenWidth, screenHeight);
  drawGameLabels(game);
}

void drawGameLabels(const GameState *game) {
  DrawText(game->currentMap.name, 24, 24, 24, RAYWHITE);

  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    const char *name = game->players[i].character.name;
    char label[128];
    snprintf(label, sizeof(label), "Player %d: %s", i + 1,
             name[0] != '\0' ? name : "Unknown");
    DrawText(label, 24, 58 + i * 24, 20, RAYWHITE);
  }
}
