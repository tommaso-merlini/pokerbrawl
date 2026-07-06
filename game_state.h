#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>

#include "map.h"

typedef enum GameScreen {
  SCREEN_MENU,
  SCREEN_GAME
} GameScreen;

typedef struct Player {
  Vector2 position;
  Vector2 velocity;
  Vector2 size;
  bool onGround;
  bool spawned;
} Player;

typedef struct GameState {
  GameScreen screen;
  MapList availableMaps;
  int selectedMapIndex;
  ArenaMap currentMap;
  Player player;
  char mapLoadError[512];
} GameState;

extern GameState gGame;

void initGameState(void);
bool startGameWithMap(int index);
void returnToMenu(void);

#endif
