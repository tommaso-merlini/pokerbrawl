#include "navigation.h"

#include <stddef.h>

bool uiSendNavigation(const UiNavigationTarget *target, int playerIndex,
                      UiNavigationCommand command) {
  if (target == NULL || target->send == NULL || playerIndex < 0) {
    return false;
  }

  target->send(target->context, playerIndex, command);
  return true;
}
