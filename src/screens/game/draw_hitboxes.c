#include "draw_internal.h"

#define HIT_MARKER_RADIUS 10.0f
#define HIT_MARKER_CROSS_SIZE 15.0f
#define HIT_MARKER_LINE_WIDTH 2.0f
#define PLAYER_HITBOX_LINE_WIDTH 3.0f

static void drawPointHitbox(Vector2 position, Color color) {
  Color fill = color;
  fill.a = 90;

  DrawCircleV(position, HIT_MARKER_RADIUS, fill);
  DrawCircleLinesV(position, HIT_MARKER_RADIUS, color);
  DrawLineEx((Vector2){position.x - HIT_MARKER_CROSS_SIZE, position.y},
             (Vector2){position.x + HIT_MARKER_CROSS_SIZE, position.y},
             HIT_MARKER_LINE_WIDTH, color);
  DrawLineEx((Vector2){position.x, position.y - HIT_MARKER_CROSS_SIZE},
             (Vector2){position.x, position.y + HIT_MARKER_CROSS_SIZE},
             HIT_MARKER_LINE_WIDTH, color);
}

void drawPlayerHitboxes(const GameState *game, int width, int height) {
  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    const Player *player = &game->players[i];
    if (!player->spawned) {
      continue;
    }

    Vector2 half = {player->size.x * 0.5f, player->size.y * 0.5f};
    Vector2 topLeft = mapToScreen(
        (Vector2){player->position.x - half.x, player->position.y - half.y},
        &game->currentMap, width, height);
    Vector2 bottomRight = mapToScreen(
        (Vector2){player->position.x + half.x, player->position.y + half.y},
        &game->currentMap, width, height);
    Rectangle bounds = {
        topLeft.x,
        topLeft.y,
        bottomRight.x - topLeft.x,
        bottomRight.y - topLeft.y,
    };
    Color fill = LIME;
    fill.a = 50;
    DrawRectangleRec(bounds, fill);
    DrawRectangleLinesEx(bounds, PLAYER_HITBOX_LINE_WIDTH, LIME);
  }
}

void drawHitboxes(const GameState *game, int width, int height) {
  for (int i = 0; i < game->hitCount; i++) {
    const Hit *hit = &game->hits[i];
    Vector2 screenPosition =
        mapToScreen(hit->position, &game->currentMap, width, height);
    Color color = hit->type == BODY ? RED : ORANGE;
    drawPointHitbox(screenPosition, color);
  }
}
