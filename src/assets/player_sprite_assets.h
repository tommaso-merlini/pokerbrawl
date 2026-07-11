#ifndef ASSETS_PLAYER_SPRITE_ASSETS_H
#define ASSETS_PLAYER_SPRITE_ASSETS_H

#include "../character.h"
#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>

#define PLAYER_SPRITE_FRAME_WIDTH 80
#define PLAYER_SPRITE_FRAME_HEIGHT 110
#define MAX_PLAYER_SPRITE_FRAMES 2

typedef enum PlayerSpriteAnimation {
  PLAYER_SPRITE_IDLE,
  PLAYER_SPRITE_WALK,
  PLAYER_SPRITE_JUMP,
  PLAYER_SPRITE_FALL,
  PLAYER_SPRITE_ANIMATION_COUNT
} PlayerSpriteAnimation;

typedef struct PlayerSpriteClip {
  int frameIndices[MAX_PLAYER_SPRITE_FRAMES];
  int frameCount;
  float frameDuration;
} PlayerSpriteClip;

typedef struct PlayerSpriteAssets {
  Texture2D sheets[CHARACTER_ID_COUNT];
  bool loaded;
} PlayerSpriteAssets;

bool loadPlayerSpriteAssets(PlayerSpriteAssets *assets, char *error,
                            size_t errorSize);
void unloadPlayerSpriteAssets(PlayerSpriteAssets *assets);
const Texture2D *getPlayerSpriteSheet(const PlayerSpriteAssets *assets,
                                      CharacterId character);
const PlayerSpriteClip *getPlayerSpriteClip(PlayerSpriteAnimation animation);
Rectangle getPlayerSpriteFrame(int frameIndex);

#endif
