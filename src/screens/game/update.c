#include "game_screen.h"

#include "../../game/game_state_internal.h"

#define BODY_HIT_DAMAGE 10
#define BODY_HIT_OFFSET 60.0f
#define BODY_KNOCKBACK_HORIZONTAL 620.0f
#define BODY_KNOCKBACK_VERTICAL 420.0f

static void createBodyHit(GameState *game, int attackerIndex) {
  const Player *attacker = &game->players[attackerIndex];
  float direction = attacker->facingLeft ? -1.0f : 1.0f;

  addHit(game, (Hit){
                   .attackerIndex = attackerIndex,
                   .type = BODY,
                   .damage = BODY_HIT_DAMAGE,
                   .position = {
                       attacker->position.x + direction * BODY_HIT_OFFSET,
                       attacker->position.y,
                   },
                   .knockback = {
                       direction * BODY_KNOCKBACK_HORIZONTAL,
                       -BODY_KNOCKBACK_VERTICAL,
                   },
               });
}

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
