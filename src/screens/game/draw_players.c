#include "draw_internal.h"

static void drawMissingSprite(const Player *player, const ArenaMap *map,
                              int width, int height) {
  Vector2 half = {player->size.x * 0.5f, player->size.y * 0.5f};
  Vector2 topLeft = mapToScreen(
      (Vector2){player->position.x - half.x, player->position.y - half.y}, map,
      width, height);
  Vector2 bottomRight = mapToScreen(
      (Vector2){player->position.x + half.x, player->position.y + half.y}, map,
      width, height);
  DrawRectangleRec((Rectangle){topLeft.x, topLeft.y, bottomRight.x - topLeft.x,
                               bottomRight.y - topLeft.y},
                   MAGENTA);
}

static void drawPlayer(const Player *player, const ArenaMap *map,
                       const PlayerRenderer *renderer,
                       const PlayerSpriteState *state, int width, int height) {
  const PlayerSpriteClip *clip = getPlayerSpriteClip(state->animation);
  PlayerSpritePose pose = clip->poses[state->frame];
  const Texture2D *texture =
      getPlayerSpritePose(&renderer->assets, player->character.id, pose);
  if (texture == NULL) {
    drawMissingSprite(player, map, width, height);
    return;
  }

  Rectangle source = {0.0f, 0.0f, (float)texture->width,
                      (float)texture->height};
  if (state->facingLeft) {
    source.width = -source.width;
  }

  float spriteHeight = player->size.y * PLAYER_VISUAL_HEIGHT_SCALE;
  float spriteWidth =
      spriteHeight * ((float)texture->width / (float)texture->height);
  Vector2 feet = {player->position.x,
                  player->position.y + player->size.y * 0.5f};
  Vector2 screenFeet = mapToScreen(feet, map, width, height);
  Vector2 screenTopLeft =
      mapToScreen((Vector2){feet.x - spriteWidth * 0.5f, feet.y - spriteHeight},
                  map, width, height);
  Rectangle destination = {
      screenFeet.x,
      screenFeet.y,
      (screenFeet.x - screenTopLeft.x) * 2.0f,
      screenFeet.y - screenTopLeft.y,
  };
  Vector2 origin = {destination.width * 0.5f, destination.height};
  DrawTexturePro(*texture, source, destination, origin, 0.0f, WHITE);
}

void drawPlayers(const GameState *game, const PlayerRenderer *renderer,
                 int width, int height) {
  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    if (game->players[i].spawned) {
      drawPlayer(&game->players[i], &game->currentMap, renderer,
                 &renderer->players[i], width, height);
    }
  }
}
