#ifndef UI_NAVIGATION_H
#define UI_NAVIGATION_H

#include <stdbool.h>

typedef enum UiNavigationCommand {
  UI_NAVIGATION_UP,
  UI_NAVIGATION_RIGHT,
  UI_NAVIGATION_DOWN,
  UI_NAVIGATION_LEFT,
  UI_NAVIGATION_CONFIRM,
  UI_NAVIGATION_BACK
} UiNavigationCommand;

// Generic destination for menu-like navigation. Input devices only know this
// interface; the screen receiving the command decides what it means.
typedef struct UiNavigationTarget {
  void *context;
  void (*send)(void *context, int playerIndex, UiNavigationCommand command);
} UiNavigationTarget;

bool uiSendNavigation(const UiNavigationTarget *target, int playerIndex,
                      UiNavigationCommand command);

#endif
