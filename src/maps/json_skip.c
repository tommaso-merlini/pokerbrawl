#include "json_reader_internal.h"

static bool skipObject(JsonReader *reader) {
  if (!jsonExpect(reader, '{')) {
    return false;
  }
  if (jsonConsume(reader, '}')) {
    return true;
  }

  while (true) {
    char key[64];
    if (!jsonString(reader, key, sizeof(key)) || !jsonExpect(reader, ':') ||
        !jsonSkipValue(reader)) {
      return false;
    }
    if (jsonConsume(reader, '}')) {
      return true;
    }
    if (!jsonExpect(reader, ',')) {
      return false;
    }
  }
}

static bool skipArray(JsonReader *reader) {
  if (!jsonExpect(reader, '[')) {
    return false;
  }
  if (jsonConsume(reader, ']')) {
    return true;
  }

  while (true) {
    if (!jsonSkipValue(reader)) {
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

bool jsonSkipValue(JsonReader *reader) {
  jsonSkipWhitespace(reader);
  switch (*reader->at) {
  case '{':
    return skipObject(reader);
  case '[':
    return skipArray(reader);
  case '"': {
    char ignored[64];
    return jsonString(reader, ignored, sizeof(ignored));
  }
  case 't':
  case 'f': {
    bool ignored = false;
    return jsonBool(reader, &ignored);
  }
  case 'n':
    return jsonLiteral(reader, "null") || jsonFail(reader, "Expected value");
  default: {
    float ignored = 0.0f;
    return jsonNumber(reader, &ignored);
  }
  }
}
