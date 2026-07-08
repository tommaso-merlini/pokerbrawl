#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>

#include "map.h"
#include "player.h"

typedef enum GameScreen {
  SCREEN_MENU,
  SCREEN_GAME
} GameScreen;

typedef struct GameState {
  GameScreen screen;
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
