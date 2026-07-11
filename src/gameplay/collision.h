#ifndef GAMEPLAY_COLLISION_H
#define GAMEPLAY_COLLISION_H

typedef struct ArenaMap ArenaMap;
typedef struct Player Player;

void resolvePlayerHorizontal(Player *player, const ArenaMap *map);
void resolvePlayerVertical(Player *player, const ArenaMap *map);

#endif
