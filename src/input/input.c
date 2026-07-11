#include "input.h"

typedef struct PlayerControls {
  int leftKey;
  int rightKey;
  int jumpKey;
} PlayerControls;

static const PlayerControls PLAYER_CONTROLS[MAX_PLAYERS] = {
    {KEY_A, KEY_D, KEY_W},
    {KEY_LEFT, KEY_RIGHT, KEY_UP},
    {KEY_J, KEY_L, KEY_I},
    {KEY_F, KEY_H, KEY_T},
};

static PlayerInput readPlayerInput(PlayerControls controls) {
  PlayerInput input = {0};

  input.move = (IsKeyDown(controls.rightKey) ? 1.0f : 0.0f) -
               (IsKeyDown(controls.leftKey) ? 1.0f : 0.0f);
  input.jumpPressed = IsKeyPressed(controls.jumpKey);
  return input;
}

InputState readInput(void) {
  InputState input = {0};

  for (int i = 0; i < MAX_PLAYERS; i++) {
    input.players[i] = readPlayerInput(PLAYER_CONTROLS[i]);
  }

  input.pointer = GetMousePosition();
  input.primaryPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
  input.backPressed = IsKeyPressed(KEY_ESCAPE);
  input.confirmPressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
  return input;
}

bool inputClicked(const InputState *input, Rectangle bounds) {
  return input->primaryPressed &&
         CheckCollisionPointRec(input->pointer, bounds);
}
