#include "player_renderer.h"

#include <math.h>

static PlayerSpriteAnimation animationForPlayer(const Player *player) {
  if (!player->onGround) {
    return player->velocity.y < 0.0f ? PLAYER_SPRITE_JUMP : PLAYER_SPRITE_FALL;
  }
  return fabsf(player->velocity.x) > 0.1f ? PLAYER_SPRITE_WALK
                                         : PLAYER_SPRITE_IDLE;
}

bool initPlayerRenderer(PlayerRenderer *renderer) {
  *renderer = (PlayerRenderer){0};
  return loadPlayerSpriteAssets(&renderer->assets, renderer->loadError,
                                sizeof(renderer->loadError));
}

void unloadPlayerRenderer(PlayerRenderer *renderer) {
  unloadPlayerSpriteAssets(&renderer->assets);
  *renderer = (PlayerRenderer){0};
}

void updatePlayerRenderer(PlayerRenderer *renderer, const GameState *game,
                          float dt) {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    PlayerSpriteState *state = &renderer->players[i];
    if (i >= game->playerCount || !game->players[i].spawned) {
      *state = (PlayerSpriteState){0};
      continue;
    }

    const Player *player = &game->players[i];
    PlayerSpriteAnimation nextAnimation = animationForPlayer(player);
    if (player->velocity.x < -0.1f) {
      state->facingLeft = true;
    } else if (player->velocity.x > 0.1f) {
      state->facingLeft = false;
    }

    if (state->animation != nextAnimation) {
      state->animation = nextAnimation;
      state->frame = 0;
      state->elapsed = 0.0f;
      continue;
    }

    const PlayerSpriteClip *clip = getPlayerSpriteClip(state->animation);
    if (clip->frameCount > 1 && clip->frameDuration > 0.0f) {
      state->elapsed += dt;
      while (state->elapsed >= clip->frameDuration) {
        state->elapsed -= clip->frameDuration;
        state->frame = (state->frame + 1) % clip->frameCount;
      }
    }
  }
}
