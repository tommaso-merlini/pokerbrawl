#include "controller.h"

#include <math.h>
#include <stdio.h>

static const int RAYLIB_BUTTONS[CONTROLLER_BUTTON_COUNT] = {
    [CONTROLLER_BUTTON_DPAD_UP] = GAMEPAD_BUTTON_LEFT_FACE_UP,
    [CONTROLLER_BUTTON_DPAD_RIGHT] = GAMEPAD_BUTTON_LEFT_FACE_RIGHT,
    [CONTROLLER_BUTTON_DPAD_DOWN] = GAMEPAD_BUTTON_LEFT_FACE_DOWN,
    [CONTROLLER_BUTTON_DPAD_LEFT] = GAMEPAD_BUTTON_LEFT_FACE_LEFT,
    [CONTROLLER_BUTTON_TRIANGLE] = GAMEPAD_BUTTON_RIGHT_FACE_UP,
    [CONTROLLER_BUTTON_CIRCLE] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,
    [CONTROLLER_BUTTON_CROSS] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN,
    [CONTROLLER_BUTTON_SQUARE] = GAMEPAD_BUTTON_RIGHT_FACE_LEFT,
    [CONTROLLER_BUTTON_L1] = GAMEPAD_BUTTON_LEFT_TRIGGER_1,
    [CONTROLLER_BUTTON_L2] = GAMEPAD_BUTTON_LEFT_TRIGGER_2,
    [CONTROLLER_BUTTON_R1] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1,
    [CONTROLLER_BUTTON_R2] = GAMEPAD_BUTTON_RIGHT_TRIGGER_2,
    [CONTROLLER_BUTTON_SHARE] = GAMEPAD_BUTTON_MIDDLE_LEFT,
    [CONTROLLER_BUTTON_PS] = GAMEPAD_BUTTON_MIDDLE,
    [CONTROLLER_BUTTON_OPTIONS] = GAMEPAD_BUTTON_MIDDLE_RIGHT,
    [CONTROLLER_BUTTON_L3] = GAMEPAD_BUTTON_LEFT_THUMB,
    [CONTROLLER_BUTTON_R3] = GAMEPAD_BUTTON_RIGHT_THUMB,
};

static int findControllerIndex(const ControllerRegistry *registry, int id) {
  for (int i = 0; i < registry->count; i++) {
    if (registry->controllers[i].id == id) {
      return i;
    }
  }

  return -1;
}

static void pollController(Controller *controller) {
  float previousHorizontal = controller->controls.axes[GAMEPAD_AXIS_LEFT_X];
  float previousVertical = controller->controls.axes[GAMEPAD_AXIS_LEFT_Y];

  for (int axis = 0; axis < CONTROLLER_AXIS_COUNT; axis++) {
    controller->controls.axes[axis] =
        GetGamepadAxisMovement(controller->id, axis);
  }

  for (int button = 0; button < CONTROLLER_BUTTON_COUNT; button++) {
    int raylibButton = RAYLIB_BUTTONS[button];
    controller->controls.buttonsDown[button] =
        IsGamepadButtonDown(controller->id, raylibButton);
    controller->controls.buttonsPressed[button] =
        IsGamepadButtonPressed(controller->id, raylibButton);
    controller->controls.buttonsReleased[button] =
        IsGamepadButtonReleased(controller->id, raylibButton);
  }

  const float axisThreshold = 0.55f;
  float horizontal = controller->controls.axes[GAMEPAD_AXIS_LEFT_X];
  float vertical = controller->controls.axes[GAMEPAD_AXIS_LEFT_Y];
  controller->navigation = (ControllerNavigation){
      .upPressed =
          controller->controls.buttonsPressed[CONTROLLER_BUTTON_DPAD_UP] ||
          (vertical <= -axisThreshold && previousVertical > -axisThreshold),
      .rightPressed =
          controller->controls.buttonsPressed[CONTROLLER_BUTTON_DPAD_RIGHT] ||
          (horizontal >= axisThreshold && previousHorizontal < axisThreshold),
      .downPressed =
          controller->controls.buttonsPressed[CONTROLLER_BUTTON_DPAD_DOWN] ||
          (vertical >= axisThreshold && previousVertical < axisThreshold),
      .leftPressed =
          controller->controls.buttonsPressed[CONTROLLER_BUTTON_DPAD_LEFT] ||
          (horizontal <= -axisThreshold && previousHorizontal > -axisThreshold),
      .confirmPressed =
          controller->controls.buttonsPressed[CONTROLLER_BUTTON_CROSS],
      .backPressed =
          controller->controls.buttonsPressed[CONTROLLER_BUTTON_CIRCLE],
  };
}

static void addController(ControllerRegistry *registry, int id) {
  if (registry->count >= MAX_CONTROLLERS) {
    return;
  }

  const char *name = GetGamepadName(id);
  Controller *controller = &registry->controllers[registry->count++];
  *controller = (Controller){.id = id};
  snprintf(controller->name, sizeof(controller->name), "%s",
           name != NULL ? name : "Unknown controller");

  TraceLog(LOG_INFO, "Controller %d connected: %s", controller->id,
           controller->name);
}

static void removeController(ControllerRegistry *registry, int index) {
  TraceLog(LOG_INFO, "Controller %d disconnected: %s",
           registry->controllers[index].id, registry->controllers[index].name);

  for (int i = index; i < registry->count - 1; i++) {
    registry->controllers[i] = registry->controllers[i + 1];
  }

  registry->count--;
  registry->controllers[registry->count] = (Controller){0};
}

void initControllerRegistry(ControllerRegistry *registry) {
  *registry = (ControllerRegistry){0};
}

void updateControllerRegistry(ControllerRegistry *registry) {
  for (int i = registry->count - 1; i >= 0; i--) {
    if (!IsGamepadAvailable(registry->controllers[i].id)) {
      removeController(registry, i);
    }
  }

  for (int id = 0; id < MAX_CONTROLLERS; id++) {
    if (IsGamepadAvailable(id) && findControllerIndex(registry, id) < 0) {
      addController(registry, id);
    }
  }

  for (int i = 0; i < registry->count; i++) {
    pollController(&registry->controllers[i]);
  }
}

const Controller *getController(const ControllerRegistry *registry,
                                int controllerIndex) {
  if (controllerIndex < 0 || controllerIndex >= registry->count) {
    return NULL;
  }

  return &registry->controllers[controllerIndex];
}

void controllerRegistrySendNavigation(const ControllerRegistry *registry,
                                      const UiNavigationTarget *target) {
  for (int i = 0; i < registry->count; i++) {
    const ControllerNavigation *navigation =
        &registry->controllers[i].navigation;

    if (navigation->upPressed) {
      uiSendNavigation(target, i, UI_NAVIGATION_UP);
    }
    if (navigation->rightPressed) {
      uiSendNavigation(target, i, UI_NAVIGATION_RIGHT);
    }
    if (navigation->downPressed) {
      uiSendNavigation(target, i, UI_NAVIGATION_DOWN);
    }
    if (navigation->leftPressed) {
      uiSendNavigation(target, i, UI_NAVIGATION_LEFT);
    }
    if (navigation->confirmPressed) {
      uiSendNavigation(target, i, UI_NAVIGATION_CONFIRM);
    }
    if (navigation->backPressed) {
      uiSendNavigation(target, i, UI_NAVIGATION_BACK);
    }
  }
}

static float controllerMovement(const Controller *controller) {
  bool left = controller->controls.buttonsDown[CONTROLLER_BUTTON_DPAD_LEFT];
  bool right = controller->controls.buttonsDown[CONTROLLER_BUTTON_DPAD_RIGHT];
  if (left || right) {
    return (right ? 1.0f : 0.0f) - (left ? 1.0f : 0.0f);
  }

  float movement = controller->controls.axes[GAMEPAD_AXIS_LEFT_X];
  const float deadzone = 0.18f;
  return fabsf(movement) >= deadzone ? movement : 0.0f;
}

void controllerRegistrySendPlayerCommands(const ControllerRegistry *registry,
                                          const PlayerCommandTarget *target) {
  for (int i = 0; i < registry->count; i++) {
    const Controller *controller = &registry->controllers[i];
    float movement = controllerMovement(controller);

    if (movement < 0.0f) {
      sendPlayerCommand(target, i, PLAYER_COMMAND_MOVE_LEFT, -movement);
    } else if (movement > 0.0f) {
      sendPlayerCommand(target, i, PLAYER_COMMAND_MOVE_RIGHT, movement);
    }
    if (controller->controls.buttonsPressed[CONTROLLER_BUTTON_CROSS]) {
      sendPlayerCommand(target, i, PLAYER_COMMAND_JUMP, 1.0f);
    }
    if (controller->controls.buttonsPressed[CONTROLLER_BUTTON_SQUARE]) {
      sendPlayerCommand(target, i, PLAYER_COMMAND_HIT, 1.0f);
    }
  }
}
