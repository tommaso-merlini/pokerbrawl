#ifndef ASSETS_PLAYER_SPRITE_ASSETS_H
#define ASSETS_PLAYER_SPRITE_ASSETS_H

#include "../character.h"
#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>

#define MAX_PLAYER_SPRITE_FRAMES 2

typedef enum PlayerSpritePose {
  PLAYER_SPRITE_POSE_IDLE,
  PLAYER_SPRITE_POSE_WALK_1,
  PLAYER_SPRITE_POSE_WALK_2,
  PLAYER_SPRITE_POSE_JUMP,
  PLAYER_SPRITE_POSE_FALL,
  PLAYER_SPRITE_POSE_COUNT
} PlayerSpritePose;

typedef enum PlayerSpriteAnimation {
  PLAYER_SPRITE_IDLE,
  PLAYER_SPRITE_WALK,
  PLAYER_SPRITE_JUMP,
  PLAYER_SPRITE_FALL,
  PLAYER_SPRITE_HURT,
  PLAYER_SPRITE_ANIMATION_COUNT
} PlayerSpriteAnimation;

typedef struct PlayerSpriteClip {
  PlayerSpritePose poses[MAX_PLAYER_SPRITE_FRAMES];
  int frameCount;
  float frameDuration;
} PlayerSpriteClip;

typedef struct CharacterSpriteAssets {
  Texture2D poses[PLAYER_SPRITE_POSE_COUNT];
  int frameWidth;
  int frameHeight;
} CharacterSpriteAssets;

typedef struct PlayerSpriteAssets {
  CharacterSpriteAssets characters[CHARACTER_ID_COUNT];
  bool loaded;
} PlayerSpriteAssets;

bool loadPlayerSpriteAssets(PlayerSpriteAssets *assets, char *error,
                            size_t errorSize);
void unloadPlayerSpriteAssets(PlayerSpriteAssets *assets);
const Texture2D *getPlayerSpritePose(const PlayerSpriteAssets *assets,
                                     CharacterId character,
                                     PlayerSpritePose pose);
const PlayerSpriteClip *getPlayerSpriteClip(PlayerSpriteAnimation animation);

#endif
