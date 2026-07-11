#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "map.h"

#include <stdbool.h>

bool parseMapListJson(const char *text, MapList *maps, char *error,
                      int errorSize);

#endif
