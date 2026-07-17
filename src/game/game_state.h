#ifndef GAME_GAME_STATE_H
#define GAME_GAME_STATE_H

#include "../character.h"
#include "../gameplay/hit.h"
#include "../gameplay/player.h"
#include "../maps/map.h"

#include <stdbool.h>

typedef enum GameMode { GAME_MODE_1V1 } GameMode;

typedef enum GameScreen {
  SCREEN_MENU,
  SCREEN_GAME,
  SCREEN_GAME_END
} GameScreen;

typedef enum MenuStep {
  MENU_STEP_MODE,
  MENU_STEP_CHARACTERS,
  MENU_STEP_MAPS
} MenuStep;

typedef struct MenuState {
  MenuStep step;
  bool navigationTransitionHandled;
} MenuState;

typedef struct UiState {
  Vector2 pointer;
} UiState;

typedef struct GameState {
  GameScreen screen;
  MenuState menu;
  UiState ui;
  GameMode selectedMode;
  Character availableCharacters[MAX_CHARACTERS];
  int characterCount;
  MapList availableMaps;
  int selectedMapIndex;
  ArenaMap currentMap;
  Player players[MAX_PLAYERS];
  int playerCount;
  int winnerPlayerIndex;
  bool debugMode;
  char mapLoadError[512];
  Hit hits[MAX_HITS];
  int hitCount;
} GameState;

void initGameState(GameState *game);
int getActivePlayerCount(const GameState *game);
bool setPlayerCharacter(GameState *game, int playerIndex, int characterIndex);
int getPlayerCharacterIndex(const GameState *game, int playerIndex);
bool playerHasCharacter(const GameState *game, int playerIndex,
                        int characterIndex);
void normalizeGameSelections(GameState *game);
bool startGameWithMap(GameState *game, int index);
void returnToMenu(GameState *game);

#endif
