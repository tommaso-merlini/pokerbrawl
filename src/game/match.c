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

static Vector2 randomSpawnPoint(const ArenaMap *map, int playerIndex,
                                int playerCount) {
  if (map->spawnpointCount > 0) {
    return map->spawnpoints[GetRandomValue(0, map->spawnpointCount - 1)];
  }
  return fallbackSpawnPoint(map, playerIndex, playerCount);
}

static bool playerOutsideArena(const Player *player, const ArenaMap *map) {
  Vector2 half = {player->size.x * 0.5f, player->size.y * 0.5f};
  return player->position.x + half.x < 0.0f ||
         player->position.x - half.x > (float)map->width ||
         player->position.y + half.y < 0.0f ||
         player->position.y - half.y > (float)map->height;
}

void handlePlayerDeaths(GameState *game) {
  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    Player *player = &game->players[i];
    if (!player->spawned ||
        (player->healthPoints > 0 &&
         !playerOutsideArena(player, &game->currentMap))) {
      continue;
    }

    player->lives--;
    if (player->lives <= 0) {
      player->lives = 0;
      player->healthPoints = 0;
      player->velocity = (Vector2){0};
      player->spawned = false;
      continue;
    }

    respawnPlayer(player,
                  randomSpawnPoint(&game->currentMap, i, game->playerCount));
  }
}

void finishMatchIfNeeded(GameState *game) {
  int remainingPlayers = 0;
  int winnerIndex = -1;

  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    if (game->players[i].spawned && game->players[i].lives > 0) {
      remainingPlayers++;
      winnerIndex = i;
    }
  }

  if (remainingPlayers <= 1) {
    game->winnerPlayerIndex = remainingPlayers == 1 ? winnerIndex : -1;
    game->screen = SCREEN_GAME_END;
  }
}

void resetPlayers(GameState *game, const ArenaMap *map) {
  clearHits(game);

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
  game->winnerPlayerIndex = -1;
  resetPlayers(game, &game->currentMap);
  game->screen = SCREEN_GAME;
  return true;
}
