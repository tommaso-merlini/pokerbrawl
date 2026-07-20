#include "keyboard.h"

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

KeyboardState readKeyboard(void) {
  KeyboardState keyboard = {0};

  for (int i = 0; i < MAX_PLAYERS; i++) {
    keyboard.players[i] = readPlayerInput(PLAYER_CONTROLS[i]);
  }

  keyboard.pointer = GetMousePosition();
  keyboard.primaryPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
  keyboard.backPressed = IsKeyPressed(KEY_ESCAPE);
  keyboard.confirmPressed =
      IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
  keyboard.navigateUpPressed = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
  keyboard.navigateRightPressed =
      IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D);
  keyboard.navigateDownPressed = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
  keyboard.navigateLeftPressed = IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A);
  keyboard.debugTogglePressed = IsKeyPressed(KEY_F3);
  return keyboard;
}

void keyboardSendPlayerCommands(const KeyboardState *keyboard,
                                 const PlayerCommandTarget *target) {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    const PlayerInput *player = &keyboard->players[i];
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

bool keyboardClicked(const KeyboardState *keyboard, Rectangle bounds) {
  return keyboard->primaryPressed &&
         CheckCollisionPointRec(keyboard->pointer, bounds);
}
