#include "game_screen.h"

#include "../../game/game_state_internal.h"

typedef struct PlayerInputCollector {
  PlayerInput players[MAX_PLAYERS];
} PlayerInputCollector;

static float clampedStrength(float strength) {
  return strength > 1.0f ? 1.0f : strength;
}

static void collectPlayerCommand(void *context, int playerIndex,
                                 PlayerCommand command, float strength) {
  PlayerInputCollector *collector = context;
  if (collector == NULL || playerIndex < 0 || playerIndex >= MAX_PLAYERS) {
    return;
  }

  PlayerInput *input = &collector->players[playerIndex];
  strength = clampedStrength(strength);
  switch (command) {
  case PLAYER_COMMAND_MOVE_LEFT:
    input->move -= strength;
    if (input->move < -1.0f) {
      input->move = -1.0f;
    }
    break;
  case PLAYER_COMMAND_MOVE_RIGHT:
    input->move += strength;
    if (input->move > 1.0f) {
      input->move = 1.0f;
    }
    break;
  case PLAYER_COMMAND_JUMP:
    input->jumpPressed = true;
    break;
  case PLAYER_COMMAND_HIT:
    input->hitPressed = true;
    break;
  case PLAYER_COMMAND_DASH:
    input->dashPressed = true;
    break;
  }
}

static PlayerCommandTarget playerInputTarget(PlayerInputCollector *collector) {
  return (PlayerCommandTarget){
      .context = collector,
      .send = collectPlayerCommand,
  };
}

void gameScreenUpdate(GameState *game, const KeyboardState *keyboard,
                      const ControllerRegistry *controllers, float dt) {
  if (!game->currentMap.loaded) {
    return;
  }

  PlayerInputCollector collector = {0};
  PlayerCommandTarget commands = playerInputTarget(&collector);
  keyboardSendPlayerCommands(keyboard, &commands);
  controllerRegistrySendPlayerCommands(controllers, &commands);

  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    if (game->players[i].spawned) {
      PlayerInput playerInput = collector.players[i];
      updatePlayer(&game->players[i], &game->currentMap, playerInput, dt);
      if (playerInput.hitPressed && game->players[i].hurtTimer <= 0.0f) {
        createBodyHit(game, i);
      }
    }
  }

  updateHits(game, dt);
  handlePlayerDeaths(game);
  finishMatchIfNeeded(game);
}
