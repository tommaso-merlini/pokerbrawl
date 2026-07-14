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

  drawArena(&game->currentMap, mapRenderer, screenWidth, screenHeight,
            game->debugMapBoxes);
  drawPlayers(game, playerRenderer, screenWidth, screenHeight);
  drawGameLabels(game);

  const char *debugLabel = game->debugMapBoxes
                               ? "F3  DEBUG BOXES: ON"
                               : "F3  DEBUG BOXES: OFF";
  Color debugColor = game->debugMapBoxes ? LIME : (Color){180, 180, 180, 180};
  int debugWidth = MeasureText(debugLabel, 18);
  DrawText(debugLabel, screenWidth - debugWidth - 20, screenHeight - 34, 18,
           debugColor);
}

void drawGameLabels(const GameState *game) {
  DrawText(game->currentMap.name, 24, 24, 24, RAYWHITE);

  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    const char *name = game->players[i].character.name;
    char nameLabel[96];
    char statusLabel[64];
    snprintf(nameLabel, sizeof(nameLabel), "P%d %s", i + 1,
             name[0] != '\0' ? name : "Unknown");
    if (game->players[i].spawned) {
      snprintf(statusLabel, sizeof(statusLabel), "  Lives: %d  HP: %d/%d",
               game->players[i].lives, game->players[i].healthPoints,
               PLAYER_MAX_HEALTH);
    } else {
      snprintf(statusLabel, sizeof(statusLabel), "  Lives: 0  ELIMINATED");
    }

    int y = 58 + i * 26;
    DrawText(nameLabel, 24, y, 20, RAYWHITE);
    Color healthColor = game->players[i].healthPoints > 50
                            ? LIME
                            : (game->players[i].healthPoints > 20 ? ORANGE
                                                                  : RED);
    DrawText(statusLabel, 24 + MeasureText(nameLabel, 20), y, 20,
             healthColor);
  }
}
