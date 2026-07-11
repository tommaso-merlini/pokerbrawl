#include "player_sprite_assets.h"

#include <stdio.h>

static const char *CHARACTER_DIRECTORIES[CHARACTER_ID_COUNT] = {
    [CHARACTER_GIORDI] = "giordi",
    [CHARACTER_CIANKI] = "cianki",
    [CHARACTER_TOMMI] = "tommi",
    [CHARACTER_PIPPO] = "pippo",
    [CHARACTER_ALESSIO] = "alessio",
};

static const char *POSE_FILENAMES[PLAYER_SPRITE_POSE_COUNT] = {
    [PLAYER_SPRITE_POSE_IDLE] = "idle.png",
    [PLAYER_SPRITE_POSE_WALK_1] = "walk1.png",
    [PLAYER_SPRITE_POSE_WALK_2] = "walk2.png",
    [PLAYER_SPRITE_POSE_JUMP] = "jump.png",
    [PLAYER_SPRITE_POSE_FALL] = "fall.png",
};

static const PlayerSpriteClip PLAYER_SPRITE_CLIPS
    [PLAYER_SPRITE_ANIMATION_COUNT] = {
        [PLAYER_SPRITE_IDLE] = {{PLAYER_SPRITE_POSE_IDLE}, 1, 0.0f},
        [PLAYER_SPRITE_WALK] =
            {{PLAYER_SPRITE_POSE_WALK_1, PLAYER_SPRITE_POSE_WALK_2}, 2, 0.14f},
        [PLAYER_SPRITE_JUMP] = {{PLAYER_SPRITE_POSE_JUMP}, 1, 0.0f},
        [PLAYER_SPRITE_FALL] = {{PLAYER_SPRITE_POSE_FALL}, 1, 0.0f},
};

static bool loadCharacterPoses(PlayerSpriteAssets *assets,
                               CharacterId character, char *error,
                               size_t errorSize) {
  CharacterSpriteAssets *characterAssets = &assets->characters[character];

  for (int pose = 0; pose < PLAYER_SPRITE_POSE_COUNT; pose++) {
    char path[256];
    snprintf(path, sizeof(path), "assets/characters/%s/%s",
             CHARACTER_DIRECTORIES[character], POSE_FILENAMES[pose]);
    Texture2D texture = LoadTexture(path);
    characterAssets->poses[pose] = texture;

    if (!IsTextureValid(texture)) {
      if (error != NULL && errorSize > 0) {
        snprintf(error, errorSize, "Could not load player pose %.390s", path);
      }
      return false;
    }

    if (pose == 0) {
      characterAssets->frameWidth = texture.width;
      characterAssets->frameHeight = texture.height;
    } else if (texture.width != characterAssets->frameWidth ||
               texture.height != characterAssets->frameHeight) {
      if (error != NULL && errorSize > 0) {
        snprintf(error, errorSize,
                 "Player poses must share a canvas size: %.370s", path);
      }
      return false;
    }
  }

  return true;
}

bool loadPlayerSpriteAssets(PlayerSpriteAssets *assets, char *error,
                            size_t errorSize) {
  *assets = (PlayerSpriteAssets){0};

  for (int character = 0; character < CHARACTER_ID_COUNT; character++) {
    if (!loadCharacterPoses(assets, (CharacterId)character, error, errorSize)) {
      unloadPlayerSpriteAssets(assets);
      return false;
    }
  }

  assets->loaded = true;
  return true;
}

void unloadPlayerSpriteAssets(PlayerSpriteAssets *assets) {
  for (int character = 0; character < CHARACTER_ID_COUNT; character++) {
    for (int pose = 0; pose < PLAYER_SPRITE_POSE_COUNT; pose++) {
      Texture2D texture = assets->characters[character].poses[pose];
      if (IsTextureValid(texture)) {
        UnloadTexture(texture);
      }
    }
  }
  *assets = (PlayerSpriteAssets){0};
}

const Texture2D *getPlayerSpritePose(const PlayerSpriteAssets *assets,
                                     CharacterId character,
                                     PlayerSpritePose pose) {
  if (!assets->loaded || character < 0 || character >= CHARACTER_ID_COUNT ||
      pose < 0 || pose >= PLAYER_SPRITE_POSE_COUNT) {
    return NULL;
  }
  return &assets->characters[character].poses[pose];
}

const PlayerSpriteClip *getPlayerSpriteClip(PlayerSpriteAnimation animation) {
  if (animation < 0 || animation >= PLAYER_SPRITE_ANIMATION_COUNT) {
    return &PLAYER_SPRITE_CLIPS[PLAYER_SPRITE_IDLE];
  }
  return &PLAYER_SPRITE_CLIPS[animation];
}
