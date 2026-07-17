#include "player_commands.h"

#include <stddef.h>

bool sendPlayerCommand(const PlayerCommandTarget *target, int playerIndex,
                       PlayerCommand command, float strength) {
  if (target == NULL || target->send == NULL || playerIndex < 0 ||
      strength <= 0.0f) {
    return false;
  }

  target->send(target->context, playerIndex, command, strength);
  return true;
}
