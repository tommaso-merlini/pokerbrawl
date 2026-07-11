#include "map_loader_internal.h"

#include <stdio.h>
#include <string.h>

static bool parseMap(JsonReader *reader, ArenaMap *map) {
  *map = (ArenaMap){.width = 800, .height = 450};
  snprintf(map->name, sizeof(map->name), "untitled");

  if (!jsonExpect(reader, '{')) {
    return false;
  }
  if (jsonConsume(reader, '}')) {
    map->loaded = true;
    return true;
  }

  while (true) {
    char key[64];
    if (!jsonString(reader, key, sizeof(key)) || !jsonExpect(reader, ':')) {
      return false;
    }

    if (strcmp(key, "name") == 0) {
      if (!jsonString(reader, map->name, sizeof(map->name))) {
        return false;
      }
    } else if (strcmp(key, "background") == 0) {
      if (!jsonString(reader, map->background, sizeof(map->background))) {
        return false;
      }
    } else if (strcmp(key, "width") == 0 || strcmp(key, "height") == 0) {
      float value = 0.0f;
      if (!jsonNumber(reader, &value)) {
        return false;
      }
      *(strcmp(key, "width") == 0 ? &map->width : &map->height) = (int)value;
    } else if (strcmp(key, "quads") == 0) {
      if (!parseMapQuads(reader, map)) {
        return false;
      }
    } else if (strcmp(key, "spawnpoints") == 0) {
      if (!parseSpawnPoints(reader, map)) {
        return false;
      }
    } else if (!jsonSkipValue(reader)) {
      return false;
    }

    if (jsonConsume(reader, '}')) {
      break;
    }
    if (!jsonExpect(reader, ',')) {
      return false;
    }
  }

  if (map->width <= 0 || map->height <= 0) {
    return jsonFail(reader, "Map width and height must be positive");
  }
  map->loaded = true;
  return true;
}

static bool parseMaps(JsonReader *reader, MapList *maps) {
  if (!jsonExpect(reader, '[') || jsonConsume(reader, ']')) {
    return jsonFail(reader, "Maps array is empty");
  }

  while (true) {
    if (maps->count < MAX_MAPS) {
      if (!parseMap(reader, &maps->maps[maps->count])) {
        return false;
      }
      maps->count++;
    } else if (!jsonSkipValue(reader)) {
      return false;
    }

    if (jsonConsume(reader, ']')) {
      return true;
    }
    if (!jsonExpect(reader, ',')) {
      return false;
    }
  }
}

bool parseMapDocument(JsonReader *reader, MapList *maps) {
  bool foundMaps = false;
  if (!jsonExpect(reader, '{') || jsonConsume(reader, '}')) {
    return jsonFail(reader, "Map file needs a maps array");
  }

  while (true) {
    char key[64];
    if (!jsonString(reader, key, sizeof(key)) || !jsonExpect(reader, ':')) {
      return false;
    }

    if (strcmp(key, "maps") == 0) {
      foundMaps = parseMaps(reader, maps);
      if (!foundMaps) {
        return false;
      }
    } else if (!jsonSkipValue(reader)) {
      return false;
    }

    if (jsonConsume(reader, '}')) {
      return foundMaps || jsonFail(reader, "Map file needs a maps array");
    }
    if (!jsonExpect(reader, ',')) {
      return false;
    }
  }
}
