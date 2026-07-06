#include "json_reader.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void jsoninit(JsonReader *reader, const char *text) {
  reader->at = text != NULL ? text : "";
  reader->error[0] = '\0';
}

bool jsonfail(JsonReader *reader, const char *message) {
  if (reader->error[0] == '\0') {
    snprintf(reader->error, sizeof(reader->error), "%s", message);
  }

  return false;
}

void jsonskipws(JsonReader *reader) {
  while (*reader->at != '\0' && isspace((unsigned char)*reader->at)) {
    reader->at++;
  }
}

bool jsonconsume(JsonReader *reader, char expected) {
  jsonskipws(reader);

  if (*reader->at != expected) {
    return false;
  }

  reader->at++;
  return true;
}

bool jsonexpect(JsonReader *reader, char expected) {
  if (jsonconsume(reader, expected)) {
    return true;
  }

  char message[64];
  snprintf(message, sizeof(message), "Expected '%c'", expected);
  return jsonfail(reader, message);
}

static bool jsonliteral(JsonReader *reader, const char *literal) {
  jsonskipws(reader);
  int length = (int)strlen(literal);

  if (strncmp(reader->at, literal, length) != 0) {
    return false;
  }

  reader->at += length;
  return true;
}

bool jsonstring(JsonReader *reader, char *out, int outSize) {
  jsonskipws(reader);

  if (outSize <= 0) {
    return jsonfail(reader, "String output buffer is empty");
  }

  if (*reader->at != '"') {
    return jsonfail(reader, "Expected string");
  }

  reader->at++;
  int length = 0;

  while (*reader->at != '\0' && *reader->at != '"') {
    char value = *reader->at++;

    if (value == '\\') {
      char escape = *reader->at++;

      switch (escape) {
      case '"':
      case '\\':
      case '/':
        value = escape;
        break;
      case 'b':
        value = '\b';
        break;
      case 'f':
        value = '\f';
        break;
      case 'n':
        value = '\n';
        break;
      case 'r':
        value = '\r';
        break;
      case 't':
        value = '\t';
        break;
      case 'u':
        for (int i = 0; i < 4; i++) {
          if (reader->at[i] == '\0' ||
              !isxdigit((unsigned char)reader->at[i])) {
            return jsonfail(reader, "Invalid unicode escape");
          }
        }
        reader->at += 4;
        value = '?';
        break;
      default:
        return jsonfail(reader, "Invalid string escape");
      }
    }

    if (length < outSize - 1) {
      out[length++] = value;
    }
  }

  if (*reader->at != '"') {
    return jsonfail(reader, "Unterminated string");
  }

  reader->at++;
  out[length] = '\0';
  return true;
}

bool jsonnumber(JsonReader *reader, float *out) {
  jsonskipws(reader);

  if (*reader->at != '-' && !isdigit((unsigned char)*reader->at)) {
    return jsonfail(reader, "Expected number");
  }

  char *end = NULL;
  float value = strtof(reader->at, &end);

  if (end == reader->at) {
    return jsonfail(reader, "Invalid number");
  }

  reader->at = end;
  *out = value;
  return true;
}

bool jsonbool(JsonReader *reader, bool *out) {
  if (jsonliteral(reader, "true")) {
    *out = true;
    return true;
  }

  if (jsonliteral(reader, "false")) {
    *out = false;
    return true;
  }

  return jsonfail(reader, "Expected boolean");
}

static bool jsonskipobject(JsonReader *reader) {
  if (!jsonexpect(reader, '{')) {
    return false;
  }

  if (jsonconsume(reader, '}')) {
    return true;
  }

  while (true) {
    char key[64];

    if (!jsonstring(reader, key, sizeof(key)) || !jsonexpect(reader, ':') ||
        !jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, '}')) {
      return true;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }
}

static bool jsonskiparray(JsonReader *reader) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  if (jsonconsume(reader, ']')) {
    return true;
  }

  while (true) {
    if (!jsonskipvalue(reader)) {
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

bool jsonskipvalue(JsonReader *reader) {
  jsonskipws(reader);

  switch (*reader->at) {
  case '{':
    return jsonskipobject(reader);
  case '[':
    return jsonskiparray(reader);
  case '"': {
    char ignored[64];
    return jsonstring(reader, ignored, sizeof(ignored));
  }
  case 't':
  case 'f': {
    bool ignored = false;
    return jsonbool(reader, &ignored);
  }
  case 'n':
    if (jsonliteral(reader, "null")) {
      return true;
    }
    break;
  default: {
    float ignored = 0.0f;
    return jsonnumber(reader, &ignored);
  }
  }

  return jsonfail(reader, "Expected value");
}
