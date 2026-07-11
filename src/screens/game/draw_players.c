#include "draw_internal.h"

typedef struct PlayerColors {
  Color fill;
  Color outline;
} PlayerColors;

static const PlayerColors PLAYER_COLORS[MAX_PLAYERS] = {
    {{35, 115, 230, 255}, {12, 45, 120, 255}},
    {{238, 118, 52, 255}, {128, 50, 18, 255}},
    {{45, 165, 95, 255}, {18, 86, 48, 255}},
    {{150, 95, 220, 255}, {75, 40, 135, 255}},
};

static void drawPlayer(const Player *player, const ArenaMap *map,
                       PlayerColors colors, int width, int height) {
  Vector2 half = {player->size.x * 0.5f, player->size.y * 0.5f};
  Vector2 topLeft = mapToScreen(
      (Vector2){player->position.x - half.x, player->position.y - half.y}, map,
      width, height);
  Vector2 bottomRight = mapToScreen(
      (Vector2){player->position.x + half.x, player->position.y + half.y}, map,
      width, height);
  Rectangle bounds = {topLeft.x, topLeft.y, bottomRight.x - topLeft.x,
                      bottomRight.y - topLeft.y};
  DrawRectangleRec(bounds, colors.fill);
  DrawRectangleLinesEx(bounds, 2.0f, colors.outline);
}

void drawPlayers(const GameState *game, int width, int height) {
  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    if (game->players[i].spawned) {
      drawPlayer(&game->players[i], &game->currentMap, PLAYER_COLORS[i], width,
                 height);
    }
  }
}
