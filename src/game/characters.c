#include "game_state_internal.h"

#include <string.h>

static const Character CHARACTER_DEFINITIONS[] = {
    {.id = CHARACTER_GIORDI, .name = "Giordi"},
    {.id = CHARACTER_CIANKI, .name = "Cianki"},
    {.id = CHARACTER_TOMMI, .name = "Tommi"},
    {.id = CHARACTER_PIPPO, .name = "Pippo"},
    {.id = CHARACTER_ALESSIO, .name = "Alessio"},
};

static int findCharacterIndex(const GameState *game, const char *name) {
  if (name == NULL || name[0] == '\0') {
    return -1;
  }

  for (int i = 0; i < game->characterCount; i++) {
    if (strcmp(game->availableCharacters[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

void initAvailableCharacters(GameState *game) {
  int count =
      (int)(sizeof(CHARACTER_DEFINITIONS) / sizeof(CHARACTER_DEFINITIONS[0]));

  for (int i = 0; i < count && i < MAX_CHARACTERS; i++) {
    game->availableCharacters[game->characterCount++] =
        CHARACTER_DEFINITIONS[i];
  }
}

Character defaultCharacter(const GameState *game) {
  return game->characterCount > 0 ? game->availableCharacters[0]
                                  : (Character){0};
}

Character normalizedCharacter(const GameState *game, Character character) {
  return findCharacterIndex(game, character.name) >= 0 ? character
                                                       : defaultCharacter(game);
}

int getActivePlayerCount(const GameState *game) {
  int count = game->playerCount > 0 ? game->playerCount : DEFAULT_PLAYER_COUNT;
  return count < MAX_PLAYERS ? count : MAX_PLAYERS;
}

bool setPlayerCharacter(GameState *game, int playerIndex, int characterIndex) {
  if (playerIndex < 0 || playerIndex >= MAX_PLAYERS || characterIndex < 0 ||
      characterIndex >= game->characterCount) {
    return false;
  }

  game->players[playerIndex].character =
      game->availableCharacters[characterIndex];
  return true;
}

int getPlayerCharacterIndex(const GameState *game, int playerIndex) {
  if (playerIndex < 0 || playerIndex >= MAX_PLAYERS) {
    return -1;
  }

  return findCharacterIndex(game, game->players[playerIndex].character.name);
}

bool playerHasCharacter(const GameState *game, int playerIndex,
                        int characterIndex) {
  if (playerIndex < 0 || playerIndex >= MAX_PLAYERS || characterIndex < 0 ||
      characterIndex >= game->characterCount) {
    return false;
  }

  return getPlayerCharacterIndex(game, playerIndex) == characterIndex;
}

void normalizeGameSelections(GameState *game) {
  game->selectedMode = GAME_MODE_1V1;
  game->playerCount = getActivePlayerCount(game);

  for (int i = 0; i < game->playerCount; i++) {
    game->players[i].character =
        normalizedCharacter(game, game->players[i].character);
  }

  if (game->availableMaps.count > 0 &&
      (game->selectedMapIndex < 0 ||
       game->selectedMapIndex >= game->availableMaps.count)) {
    game->selectedMapIndex = 0;
  }
}
