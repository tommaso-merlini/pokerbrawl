#ifndef INPUT_INPUT_H
#define INPUT_INPUT_H

#include "../game/constants.h"
#include "../gameplay/player_commands.h"
#include "raylib.h"

#include <stdbool.h>

typedef struct PlayerInput {
  float move;
  bool jumpPressed;
  bool hitPressed;
  bool dashPressed;
} PlayerInput;

typedef struct InputState {
  PlayerInput players[MAX_PLAYERS];
  Vector2 pointer;
  bool primaryPressed;
  bool backPressed;
  bool confirmPressed;
  bool navigateUpPressed;
  bool navigateRightPressed;
  bool navigateDownPressed;
  bool navigateLeftPressed;
  bool debugTogglePressed;
} InputState;

InputState readInput(void);
void inputSendPlayerCommands(const InputState *input,
                             const PlayerCommandTarget *target);
bool inputClicked(const InputState *input, Rectangle bounds);

#endif
