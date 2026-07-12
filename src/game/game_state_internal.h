#ifndef GAME_GAME_STATE_INTERNAL_H
#define GAME_GAME_STATE_INTERNAL_H

#include "game_state.h"

void initAvailableCharacters(GameState *game);
Character defaultCharacter(const GameState *game);
Character normalizedCharacter(const GameState *game, Character character);
void resetPlayers(GameState *game, const ArenaMap *map);
void handlePlayerDeaths(GameState *game);

#endif
