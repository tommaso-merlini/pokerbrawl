#include "json_reader_internal.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void jsonInit(JsonReader *reader, const char *text) {
  reader->at = text != NULL ? text : "";
  reader->error[0] = '\0';
}

bool jsonFail(JsonReader *reader, const char *message) {
  if (reader->error[0] == '\0') {
    snprintf(reader->error, sizeof(reader->error), "%s", message);
  }
  return false;
}

void jsonSkipWhitespace(JsonReader *reader) {
  while (*reader->at != '\0' && isspace((unsigned char)*reader->at)) {
    reader->at++;
  }
}

bool jsonConsume(JsonReader *reader, char expected) {
  jsonSkipWhitespace(reader);
  if (*reader->at != expected) {
    return false;
  }
  reader->at++;
  return true;
}

bool jsonExpect(JsonReader *reader, char expected) {
  if (jsonConsume(reader, expected)) {
    return true;
  }

  char message[64];
  snprintf(message, sizeof(message), "Expected '%c'", expected);
  return jsonFail(reader, message);
}

bool jsonLiteral(JsonReader *reader, const char *literal) {
  jsonSkipWhitespace(reader);
  int length = (int)strlen(literal);
  if (strncmp(reader->at, literal, length) != 0) {
    return false;
  }
  reader->at += length;
  return true;
}

bool jsonNumber(JsonReader *reader, float *out) {
  jsonSkipWhitespace(reader);
  if (*reader->at != '-' && !isdigit((unsigned char)*reader->at)) {
    return jsonFail(reader, "Expected number");
  }

  char *end = NULL;
  float value = strtof(reader->at, &end);
  if (end == reader->at) {
    return jsonFail(reader, "Invalid number");
  }
  reader->at = end;
  *out = value;
  return true;
}

bool jsonBool(JsonReader *reader, bool *out) {
  if (jsonLiteral(reader, "true")) {
    *out = true;
    return true;
  }
  if (jsonLiteral(reader, "false")) {
    *out = false;
    return true;
  }
  return jsonFail(reader, "Expected boolean");
}
