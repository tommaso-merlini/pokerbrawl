#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>

#include "map.h"
#include "player.h"

#define MAX_CHARACTERS 8
#define MAX_CHARACTER_NAME 64

typedef enum GameMode {
  GAME_MODE_1V1
} GameMode;

typedef struct Character {
  char name[MAX_CHARACTER_NAME];
} Character;

typedef struct PlayerSetup {
  int selectedCharacterIndex;
} PlayerSetup;

typedef enum GameScreen {
  SCREEN_MENU,
  SCREEN_GAME
} GameScreen;

typedef struct GameState {
  GameScreen screen;
  GameMode selectedMode;
  Character availableCharacters[MAX_CHARACTERS];
  int characterCount;
  PlayerSetup playerSetups[MAX_PLAYERS];
  MapList availableMaps;
  int selectedMapIndex;
  ArenaMap currentMap;
  Player players[MAX_PLAYERS];
  int playerCount;
  char mapLoadError[512];
} GameState;

extern GameState gGame;

void initGameState(void);
bool startGameWithMap(int index);
void returnToMenu(void);

#endif
