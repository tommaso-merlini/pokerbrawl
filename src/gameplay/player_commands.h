#ifndef GAMEPLAY_PLAYER_COMMANDS_H
#define GAMEPLAY_PLAYER_COMMANDS_H

#include <stdbool.h>

typedef enum PlayerCommand {
  PLAYER_COMMAND_MOVE_LEFT,
  PLAYER_COMMAND_MOVE_RIGHT,
  PLAYER_COMMAND_JUMP,
  PLAYER_COMMAND_HIT,
  PLAYER_COMMAND_DASH
} PlayerCommand;

// Input devices only send commands through this interface. The gameplay
// decides how commands are combined and applied to each player.
typedef struct PlayerCommandTarget {
  void *context;
  void (*send)(void *context, int playerIndex, PlayerCommand command,
               float strength);
} PlayerCommandTarget;

bool sendPlayerCommand(const PlayerCommandTarget *target, int playerIndex,
                       PlayerCommand command, float strength);

#endif
