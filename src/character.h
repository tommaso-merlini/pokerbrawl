#ifndef CHARACTER_H
#define CHARACTER_H

#define MAX_CHARACTERS 8
#define MAX_CHARACTER_NAME 64

typedef enum CharacterId {
  CHARACTER_GIORDI,
  CHARACTER_CIANKI,
  CHARACTER_TOMMI,
  CHARACTER_PIPPO,
  CHARACTER_ALESSIO,
  CHARACTER_ID_COUNT
} CharacterId;

typedef struct Character {
  CharacterId id;
  char name[MAX_CHARACTER_NAME];
} Character;

#endif
