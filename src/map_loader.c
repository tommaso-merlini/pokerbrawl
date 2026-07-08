#include "map_loader.h"

#include "json_reader.h"

#include <stdio.h>
#include <string.h>

static void seterror(char *error, int errorSize, const char *message) {
  if (error != NULL && errorSize > 0) {
    snprintf(error, errorSize, "%s", message);
  }
}

static bool parsepoint(JsonReader *reader, Vector2 *point) {
  float x = 0.0f;
  float y = 0.0f;

  if (!jsonexpect(reader, '[') || !jsonnumber(reader, &x) ||
      !jsonexpect(reader, ',') || !jsonnumber(reader, &y) ||
      !jsonexpect(reader, ']')) {
    return false;
  }

  point->x = x;
  point->y = y;
  return true;
}

static bool parsequadpoints(JsonReader *reader, Vector2 points[4]) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  for (int i = 0; i < 4; i++) {
    if (i > 0 && !jsonexpect(reader, ',')) {
      return false;
    }

    if (!parsepoint(reader, &points[i])) {
      return false;
    }
  }

  return jsonexpect(reader, ']');
}

static bool parsequad(JsonReader *reader, MapQuad *quad) {
  memset(quad, 0, sizeof(*quad));
  bool hasPoints = false;

  if (!jsonexpect(reader, '{')) {
    return false;
  }

  if (jsonconsume(reader, '}')) {
    return jsonfail(reader, "Quad needs points");
  }

  while (true) {
    char key[64];

    if (!jsonstring(reader, key, sizeof(key)) || !jsonexpect(reader, ':')) {
      return false;
    }

    if (strcmp(key, "id") == 0) {
      if (!jsonstring(reader, quad->id, sizeof(quad->id))) {
        return false;
      }
    } else if (strcmp(key, "collidable") == 0) {
      if (!jsonbool(reader, &quad->collidable)) {
        return false;
      }
    } else if (strcmp(key, "points") == 0) {
      if (!parsequadpoints(reader, quad->points)) {
        return false;
      }
      hasPoints = true;
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, '}')) {
      break;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }

  if (!hasPoints) {
    return jsonfail(reader, "Quad needs points");
  }

  return true;
}

static bool parsequads(JsonReader *reader, ArenaMap *map) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  if (jsonconsume(reader, ']')) {
    return true;
  }

  while (true) {
    if (map->quadCount < MAX_MAP_QUADS) {
      if (!parsequad(reader, &map->quads[map->quadCount])) {
        return false;
      }
      map->quadCount++;
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, ']')) {
      return true;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }
}

static bool parsespawnpoints(JsonReader *reader, ArenaMap *map) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  if (jsonconsume(reader, ']')) {
    return true;
  }

  while (true) {
    if (map->spawnpointCount < MAX_MAP_SPAWNPOINTS) {
      if (!parsepoint(reader, &map->spawnpoints[map->spawnpointCount])) {
        return false;
      }
      map->spawnpointCount++;
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, ']')) {
      return true;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }
}

static bool parsemap(JsonReader *reader, ArenaMap *map) {
  memset(map, 0, sizeof(*map));
  snprintf(map->name, sizeof(map->name), "untitled");
  map->width = 800;
  map->height = 450;

  if (!jsonexpect(reader, '{')) {
    return false;
  }

  if (jsonconsume(reader, '}')) {
    map->loaded = true;
    return true;
  }

  while (true) {
    char key[64];

    if (!jsonstring(reader, key, sizeof(key)) || !jsonexpect(reader, ':')) {
      return false;
    }

    if (strcmp(key, "name") == 0) {
      if (!jsonstring(reader, map->name, sizeof(map->name))) {
        return false;
      }
    } else if (strcmp(key, "width") == 0) {
      float width = 0.0f;
      if (!jsonnumber(reader, &width)) {
        return false;
      }
      map->width = (int)width;
    } else if (strcmp(key, "height") == 0) {
      float height = 0.0f;
      if (!jsonnumber(reader, &height)) {
        return false;
      }
      map->height = (int)height;
    } else if (strcmp(key, "quads") == 0) {
      if (!parsequads(reader, map)) {
        return false;
      }
    } else if (strcmp(key, "spawnpoints") == 0) {
      if (!parsespawnpoints(reader, map)) {
        return false;
      }
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, '}')) {
      break;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }

  if (map->width <= 0 || map->height <= 0) {
    return jsonfail(reader, "Map width and height must be positive");
  }

  map->loaded = true;
  return true;
}

static bool parsemapsarray(JsonReader *reader, MapList *maps) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  if (jsonconsume(reader, ']')) {
    return jsonfail(reader, "Maps array is empty");
  }

  while (true) {
    if (maps->count < MAX_MAPS) {
      if (!parsemap(reader, &maps->maps[maps->count])) {
        return false;
      }
      maps->count++;
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, ']')) {
      return true;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }
}

static bool parsemapfile(JsonReader *reader, MapList *maps) {
  bool foundMaps = false;

  if (!jsonexpect(reader, '{')) {
    return false;
  }

  if (jsonconsume(reader, '}')) {
    return jsonfail(reader, "Map file needs a maps array");
  }

  while (true) {
    char key[64];

    if (!jsonstring(reader, key, sizeof(key)) || !jsonexpect(reader, ':')) {
      return false;
    }

    if (strcmp(key, "maps") == 0) {
      if (!parsemapsarray(reader, maps)) {
        return false;
      }
      foundMaps = true;
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, '}')) {
      break;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }

  if (!foundMaps) {
    return jsonfail(reader, "Map file needs a maps array");
  }

  return true;
}

bool parsemaplistjson(const char *text, MapList *maps, char *error,
                      int errorSize) {
  seterror(error, errorSize, "");

  if (maps == NULL) {
    seterror(error, errorSize, "Missing map output");
    return false;
  }

  if (text == NULL) {
    seterror(error, errorSize, "Missing JSON text");
    return false;
  }

  MapList parsed = {0};
  JsonReader reader;
  jsoninit(&reader, text);

  bool ok = parsemapfile(&reader, &parsed);

  if (ok) {
    jsonskipws(&reader);

    if (*reader.at != '\0') {
      ok = jsonfail(&reader, "Unexpected data after JSON root");
    }
  }

  if (!ok) {
    seterror(error, errorSize,
             reader.error[0] != '\0' ? reader.error : "Invalid map JSON");
    return false;
  }

  *maps = parsed;
  return true;
}
