#ifndef INPUT_CONTROLLER_H
#define INPUT_CONTROLLER_H

#include "../gameplay/player_commands.h"
#include "../ui/navigation.h"
#include "raylib.h"

#include <stdbool.h>

#define MAX_CONTROLLERS 4
#define CONTROLLER_NAME_CAPACITY 128
#define CONTROLLER_AXIS_COUNT (GAMEPAD_AXIS_RIGHT_TRIGGER + 1)

typedef enum ControllerButton {
  CONTROLLER_BUTTON_DPAD_UP,
  CONTROLLER_BUTTON_DPAD_RIGHT,
  CONTROLLER_BUTTON_DPAD_DOWN,
  CONTROLLER_BUTTON_DPAD_LEFT,
  CONTROLLER_BUTTON_TRIANGLE,
  CONTROLLER_BUTTON_CIRCLE,
  CONTROLLER_BUTTON_CROSS,
  CONTROLLER_BUTTON_SQUARE,
  CONTROLLER_BUTTON_L1,
  CONTROLLER_BUTTON_L2,
  CONTROLLER_BUTTON_R1,
  CONTROLLER_BUTTON_R2,
  CONTROLLER_BUTTON_SHARE,
  CONTROLLER_BUTTON_PS,
  CONTROLLER_BUTTON_OPTIONS,
  CONTROLLER_BUTTON_L3,
  CONTROLLER_BUTTON_R3,
  CONTROLLER_BUTTON_COUNT
} ControllerButton;

typedef struct ControllerControls {
  float axes[CONTROLLER_AXIS_COUNT];
  bool buttonsDown[CONTROLLER_BUTTON_COUNT];
  bool buttonsPressed[CONTROLLER_BUTTON_COUNT];
  bool buttonsReleased[CONTROLLER_BUTTON_COUNT];
} ControllerControls;

typedef struct ControllerNavigation {
  bool upPressed;
  bool rightPressed;
  bool downPressed;
  bool leftPressed;
  bool confirmPressed;
  bool backPressed;
} ControllerNavigation;

typedef struct Controller {
  int id;
  char name[CONTROLLER_NAME_CAPACITY];
  ControllerControls controls;
  ControllerNavigation navigation;
} Controller;

typedef struct ControllerRegistry {
  Controller controllers[MAX_CONTROLLERS];
  int count;
} ControllerRegistry;

void initControllerRegistry(ControllerRegistry *registry);
void updateControllerRegistry(ControllerRegistry *registry);
void controllerRegistrySendNavigation(const ControllerRegistry *registry,
                                      const UiNavigationTarget *target);
void controllerRegistrySendPlayerCommands(const ControllerRegistry *registry,
                                          const PlayerCommandTarget *target);

const Controller *getController(const ControllerRegistry *registry,
                                int controllerIndex);

#endif
