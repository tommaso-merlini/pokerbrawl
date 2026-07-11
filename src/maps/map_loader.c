#include "map_loader.h"

#include "map_loader_internal.h"

#include <stdio.h>

static void setError(char *error, int errorSize, const char *message) {
  if (error != NULL && errorSize > 0) {
    snprintf(error, errorSize, "%s", message);
  }
}

bool parseMapListJson(const char *text, MapList *maps, char *error,
                      int errorSize) {
  setError(error, errorSize, "");
  if (maps == NULL || text == NULL) {
    setError(error, errorSize,
             maps == NULL ? "Missing map output" : "Missing JSON text");
    return false;
  }

  MapList parsed = {0};
  JsonReader reader;
  jsonInit(&reader, text);
  bool ok = parseMapDocument(&reader, &parsed);

  if (ok) {
    jsonSkipWhitespace(&reader);
    ok = *reader.at == '\0' ||
         jsonFail(&reader, "Unexpected data after JSON root");
  }
  if (!ok) {
    setError(error, errorSize,
             reader.error[0] != '\0' ? reader.error : "Invalid map JSON");
    return false;
  }

  *maps = parsed;
  return true;
}
