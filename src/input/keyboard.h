#ifndef INPUT_KEYBOARD_H
#define INPUT_KEYBOARD_H

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

typedef struct KeyboardState {
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
} KeyboardState;

KeyboardState readKeyboard(void);
void keyboardSendPlayerCommands(const KeyboardState *keyboard,
                                const PlayerCommandTarget *target);
bool keyboardClicked(const KeyboardState *keyboard, Rectangle bounds);

#endif
