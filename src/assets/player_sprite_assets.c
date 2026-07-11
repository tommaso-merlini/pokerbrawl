#include "player_sprite_assets.h"

#include <stdio.h>

#define PLAYER_SPRITE_ROOT                                                   \
  "assets/vendor/kenney-platformer-characters/PNG"

static const char *PLAYER_SPRITE_PATHS[CHARACTER_ID_COUNT] = {
    [CHARACTER_GIORDI] = PLAYER_SPRITE_ROOT "/Player/player_tilesheet.png",
    [CHARACTER_CIANKI] = PLAYER_SPRITE_ROOT "/Female/female_tilesheet.png",
    [CHARACTER_TOMMI] =
        PLAYER_SPRITE_ROOT "/Adventurer/adventurer_tilesheet.png",
    [CHARACTER_PIPPO] = PLAYER_SPRITE_ROOT "/Soldier/soldier_tilesheet.png",
    [CHARACTER_ALESSIO] = PLAYER_SPRITE_ROOT "/Zombie/zombie_tilesheet.png",
};

// The Kenney sheet is a 9 x 3 row-major grid. These indices were verified
// against the separate pose PNGs shipped in the pack.
static const PlayerSpriteClip PLAYER_SPRITE_CLIPS
    [PLAYER_SPRITE_ANIMATION_COUNT] = {
        [PLAYER_SPRITE_IDLE] = {{0, 0}, 1, 0.0f},
        [PLAYER_SPRITE_WALK] = {{9, 10}, 2, 0.14f},
        [PLAYER_SPRITE_JUMP] = {{1, 0}, 1, 0.0f},
        [PLAYER_SPRITE_FALL] = {{2, 0}, 1, 0.0f},
};

bool loadPlayerSpriteAssets(PlayerSpriteAssets *assets, char *error,
                            size_t errorSize) {
  *assets = (PlayerSpriteAssets){0};

  for (int i = 0; i < CHARACTER_ID_COUNT; i++) {
    assets->sheets[i] = LoadTexture(PLAYER_SPRITE_PATHS[i]);
    if (!IsTextureValid(assets->sheets[i]) ||
        assets->sheets[i].width != PLAYER_SPRITE_FRAME_WIDTH * 9 ||
        assets->sheets[i].height != PLAYER_SPRITE_FRAME_HEIGHT * 3) {
      if (error != NULL && errorSize > 0) {
        snprintf(error, errorSize, "Invalid player spritesheet %.380s",
                 PLAYER_SPRITE_PATHS[i]);
      }
      unloadPlayerSpriteAssets(assets);
      return false;
    }
  }

  assets->loaded = true;
  return true;
}

void unloadPlayerSpriteAssets(PlayerSpriteAssets *assets) {
  for (int i = 0; i < CHARACTER_ID_COUNT; i++) {
    if (IsTextureValid(assets->sheets[i])) {
      UnloadTexture(assets->sheets[i]);
    }
  }
  *assets = (PlayerSpriteAssets){0};
}

const Texture2D *getPlayerSpriteSheet(const PlayerSpriteAssets *assets,
                                      CharacterId character) {
  if (!assets->loaded || character < 0 || character >= CHARACTER_ID_COUNT) {
    return NULL;
  }
  return &assets->sheets[character];
}

const PlayerSpriteClip *getPlayerSpriteClip(PlayerSpriteAnimation animation) {
  if (animation < 0 || animation >= PLAYER_SPRITE_ANIMATION_COUNT) {
    return &PLAYER_SPRITE_CLIPS[PLAYER_SPRITE_IDLE];
  }
  return &PLAYER_SPRITE_CLIPS[animation];
}

Rectangle getPlayerSpriteFrame(int frameIndex) {
  const int columns = 9;
  return (Rectangle){
      (float)(frameIndex % columns) * PLAYER_SPRITE_FRAME_WIDTH,
      (float)(frameIndex / columns) * PLAYER_SPRITE_FRAME_HEIGHT,
      PLAYER_SPRITE_FRAME_WIDTH,
      PLAYER_SPRITE_FRAME_HEIGHT,
  };
}
