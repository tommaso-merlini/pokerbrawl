#ifndef SCREENS_GAME_PLAYER_RENDERER_H
#define SCREENS_GAME_PLAYER_RENDERER_H

#include "../../assets/player_sprite_assets.h"
#include "../../game/game_state.h"

typedef struct PlayerSpriteState {
  PlayerSpriteAnimation animation;
  int frame;
  float elapsed;
  bool facingLeft;
} PlayerSpriteState;

typedef struct PlayerRenderer {
  PlayerSpriteAssets assets;
  PlayerSpriteState players[MAX_PLAYERS];
  char loadError[512];
} PlayerRenderer;

bool initPlayerRenderer(PlayerRenderer *renderer);
void unloadPlayerRenderer(PlayerRenderer *renderer);
void updatePlayerRenderer(PlayerRenderer *renderer, const GameState *game,
                          float dt);

#endif
