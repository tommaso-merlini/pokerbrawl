#include "game_screen.h"

#include "../../game/game_state_internal.h"

void gameScreenUpdate(GameState *game, const InputState *input, float dt) {
  if (!game->currentMap.loaded) {
    return;
  }

  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    if (game->players[i].spawned) {
      updatePlayer(&game->players[i], &game->currentMap, input->players[i], dt);
      if (input->players[i].attackPressed &&
          game->players[i].hurtTimer <= 0.0f) {
        createBodyHit(game, i);
      }
    }
  }

  updateHits(game, dt);
  handlePlayerDeaths(game);
  finishMatchIfNeeded(game);
}
