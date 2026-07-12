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
    char nameLabel[96];
    char healthLabel[32];
    snprintf(nameLabel, sizeof(nameLabel), "P%d %s", i + 1,
             name[0] != '\0' ? name : "Unknown");
    snprintf(healthLabel, sizeof(healthLabel), "  HP: %d/100",
             game->players[i].healthPoints);

    int y = 58 + i * 26;
    DrawText(nameLabel, 24, y, 20, RAYWHITE);
    Color healthColor = game->players[i].healthPoints > 50
                            ? LIME
                            : (game->players[i].healthPoints > 20 ? ORANGE
                                                                  : RED);
    DrawText(healthLabel, 24 + MeasureText(nameLabel, 20), y, 20,
             healthColor);
  }
}
