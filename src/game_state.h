#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>

#include "character.h"
#include "map.h"
#include "player.h"

typedef enum GameMode {
  GAME_MODE_1V1
} GameMode;

typedef enum GameScreen {
  SCREEN_MENU,
  SCREEN_GAME
} GameScreen;

typedef struct GameState {
  GameScreen screen;
  GameMode selectedMode;
  Character availableCharacters[MAX_CHARACTERS];
  int characterCount;
  MapList availableMaps;
  int selectedMapIndex;
  ArenaMap currentMap;
  Player players[MAX_PLAYERS];
  int playerCount;
  char mapLoadError[512];
} GameState;

extern GameState gGame;

void initGameState(void);
int getActivePlayerCount(void);
bool setPlayerCharacter(int playerIndex, int characterIndex);
bool playerHasCharacter(int playerIndex, int characterIndex);
void normalizeGameSelections(void);
bool startGameWithMap(int index);
void returnToMenu(void);

#endif
