#ifndef INPUT_INPUT_H
#define INPUT_INPUT_H

#include "../game/constants.h"
#include "raylib.h"

#include <stdbool.h>

typedef struct PlayerInput {
  float move;
  bool jumpPressed;
  bool attackPressed;
} PlayerInput;

typedef struct InputState {
  PlayerInput players[MAX_PLAYERS];
  Vector2 pointer;
  bool primaryPressed;
  bool backPressed;
  bool confirmPressed;
} InputState;

InputState readInput(void);
bool inputClicked(const InputState *input, Rectangle bounds);

#endif
