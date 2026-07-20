#include "input.h"

typedef struct PlayerControls {
  int leftKey;
  int rightKey;
  int jumpKey;
  int hitKey;
  int dashKey;
} PlayerControls;

static const PlayerControls PLAYER_CONTROLS[MAX_PLAYERS] = {
    {KEY_A, KEY_D, KEY_W, KEY_SPACE, KEY_LEFT_SHIFT},
    {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_RIGHT_CONTROL, KEY_RIGHT_SHIFT},
    {KEY_J, KEY_L, KEY_I, KEY_K, KEY_O},
    {KEY_F, KEY_H, KEY_T, KEY_G, KEY_Y},
};

static PlayerInput readPlayerInput(PlayerControls controls) {
  PlayerInput input = {0};

  input.move = (IsKeyDown(controls.rightKey) ? 1.0f : 0.0f) -
               (IsKeyDown(controls.leftKey) ? 1.0f : 0.0f);
  input.jumpPressed = IsKeyPressed(controls.jumpKey);
  input.hitPressed = IsKeyPressed(controls.hitKey);
  input.dashPressed = IsKeyPressed(controls.dashKey);
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
  input.navigateUpPressed = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
  input.navigateRightPressed = IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D);
  input.navigateDownPressed = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
  input.navigateLeftPressed = IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A);
  input.debugTogglePressed = IsKeyPressed(KEY_F3);
  return input;
}

void inputSendPlayerCommands(const InputState *input,
                             const PlayerCommandTarget *target) {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    const PlayerInput *player = &input->players[i];
    if (player->move < 0.0f) {
      sendPlayerCommand(target, i, PLAYER_COMMAND_MOVE_LEFT, -player->move);
    } else if (player->move > 0.0f) {
      sendPlayerCommand(target, i, PLAYER_COMMAND_MOVE_RIGHT, player->move);
    }
    if (player->jumpPressed) {
      sendPlayerCommand(target, i, PLAYER_COMMAND_JUMP, 1.0f);
    }
    if (player->hitPressed) {
      sendPlayerCommand(target, i, PLAYER_COMMAND_HIT, 1.0f);
    }
    if (player->dashPressed) {
      sendPlayerCommand(target, i, PLAYER_COMMAND_DASH, 1.0f);
    }
  }
}

bool inputClicked(const InputState *input, Rectangle bounds) {
  return input->primaryPressed &&
         CheckCollisionPointRec(input->pointer, bounds);
}
