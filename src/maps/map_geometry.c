#include "map_loader_internal.h"

#include <string.h>

static bool parsePoint(JsonReader *reader, Vector2 *point) {
  float x = 0.0f;
  float y = 0.0f;
  if (!jsonExpect(reader, '[') || !jsonNumber(reader, &x) ||
      !jsonExpect(reader, ',') || !jsonNumber(reader, &y) ||
      !jsonExpect(reader, ']')) {
    return false;
  }
  *point = (Vector2){x, y};
  return true;
}

static bool parseQuadPoints(JsonReader *reader, Vector2 points[4]) {
  if (!jsonExpect(reader, '[')) {
    return false;
  }
  for (int i = 0; i < 4; i++) {
    if ((i > 0 && !jsonExpect(reader, ',')) ||
        !parsePoint(reader, &points[i])) {
      return false;
    }
  }
  return jsonExpect(reader, ']');
}

static bool parseQuad(JsonReader *reader, MapQuad *quad) {
  *quad = (MapQuad){0};
  bool hasPoints = false;
  if (!jsonExpect(reader, '{') || jsonConsume(reader, '}')) {
    return jsonFail(reader, "Quad needs points");
  }

  while (true) {
    char key[64];
    if (!jsonString(reader, key, sizeof(key)) || !jsonExpect(reader, ':')) {
      return false;
    }

    if (strcmp(key, "id") == 0) {
      if (!jsonString(reader, quad->id, sizeof(quad->id))) {
        return false;
      }
    } else if (strcmp(key, "collidable") == 0) {
      if (!jsonBool(reader, &quad->collidable)) {
        return false;
      }
    } else if (strcmp(key, "points") == 0) {
      hasPoints = parseQuadPoints(reader, quad->points);
      if (!hasPoints) {
        return false;
      }
    } else if (!jsonSkipValue(reader)) {
      return false;
    }

    if (jsonConsume(reader, '}')) {
      return hasPoints || jsonFail(reader, "Quad needs points");
    }
    if (!jsonExpect(reader, ',')) {
      return false;
    }
  }
}

bool parseMapQuads(JsonReader *reader, ArenaMap *map) {
  if (!jsonExpect(reader, '[')) {
    return false;
  }
  if (jsonConsume(reader, ']')) {
    return true;
  }

  while (true) {
    if (map->quadCount < MAX_MAP_QUADS) {
      if (!parseQuad(reader, &map->quads[map->quadCount])) {
        return false;
      }
      map->quadCount++;
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

bool parseSpawnPoints(JsonReader *reader, ArenaMap *map) {
  if (!jsonExpect(reader, '[')) {
    return false;
  }
  if (jsonConsume(reader, ']')) {
    return true;
  }

  while (true) {
    if (map->spawnpointCount < MAX_MAP_SPAWNPOINTS) {
      if (!parsePoint(reader, &map->spawnpoints[map->spawnpointCount])) {
        return false;
      }
      map->spawnpointCount++;
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
