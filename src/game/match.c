#include "game_state_internal.h"

#define FALLBACK_SPAWN_SPACING 54.0f

static int playerCountForMode(GameMode mode) {
  switch (mode) {
  case GAME_MODE_1V1:
    return 2;
  }
  return DEFAULT_PLAYER_COUNT;
}

static Vector2 fallbackSpawnPoint(const ArenaMap *map, int playerIndex,
                                  int playerCount) {
  float offset = ((float)playerIndex - ((float)playerCount - 1.0f) * 0.5f) *
                 FALLBACK_SPAWN_SPACING;
  return (Vector2){(float)map->width * 0.5f + offset,
                   (float)map->height * 0.5f};
}

void resetPlayers(GameState *game, const ArenaMap *map) {
  Character characters[MAX_PLAYERS] = {0};
  for (int i = 0; i < MAX_PLAYERS; i++) {
    characters[i] = normalizedCharacter(game, game->players[i].character);
    game->players[i] = (Player){.character = characters[i]};
  }

  game->playerCount = playerCountForMode(game->selectedMode);
  if (game->playerCount > MAX_PLAYERS) {
    game->playerCount = MAX_PLAYERS;
  }

  for (int i = 0; i < game->playerCount; i++) {
    Vector2 spawn = i < map->spawnpointCount
                        ? map->spawnpoints[i]
                        : fallbackSpawnPoint(map, i, game->playerCount);
    initPlayer(&game->players[i], spawn, characters[i]);
  }
}

bool startGameWithMap(GameState *game, int index) {
  if (index < 0 || index >= game->availableMaps.count) {
    return false;
  }

  game->selectedMapIndex = index;
  game->currentMap = game->availableMaps.maps[index];
  resetPlayers(game, &game->currentMap);
  game->screen = SCREEN_GAME;
  return true;
}
