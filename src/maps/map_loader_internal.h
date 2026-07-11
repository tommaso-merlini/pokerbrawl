#ifndef MAPS_MAP_LOADER_INTERNAL_H
#define MAPS_MAP_LOADER_INTERNAL_H

#include "json_reader.h"
#include "map.h"

bool parseMapQuads(JsonReader *reader, ArenaMap *map);
bool parseSpawnPoints(JsonReader *reader, ArenaMap *map);
bool parseMapDocument(JsonReader *reader, MapList *maps);

#endif
