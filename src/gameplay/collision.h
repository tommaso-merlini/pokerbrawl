#ifndef GAMEPLAY_COLLISION_H
#define GAMEPLAY_COLLISION_H

#include <stdbool.h>

typedef struct ArenaMap ArenaMap;
typedef struct Player Player;

// The players a moving player can collide with. Players whose bit is set in
// ignoreMask are passed through (they were already overlapping).
typedef struct PlayerList {
  const Player *players;
  int count;
  unsigned ignoreMask;
} PlayerList;

void resolvePlayerHorizontal(Player *player, const ArenaMap *map,
                             const PlayerList *others, float moveX);
void resolvePlayerVertical(Player *player, const ArenaMap *map,
                           const PlayerList *others, float previousBottom);

unsigned overlappingPlayersMask(const Player *player, const PlayerList *others);
float separationPush(const Player *player, const PlayerList *others,
                     unsigned overlapMask, float maxPush);

#endif
