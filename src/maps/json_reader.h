#ifndef JSON_READER_H
#define JSON_READER_H

#include <stdbool.h>

typedef struct JsonReader {
  const char *at;
  char error[256];
} JsonReader;

void jsonInit(JsonReader *reader, const char *text);
bool jsonFail(JsonReader *reader, const char *message);
void jsonSkipWhitespace(JsonReader *reader);
bool jsonConsume(JsonReader *reader, char expected);
bool jsonExpect(JsonReader *reader, char expected);
bool jsonString(JsonReader *reader, char *out, int outSize);
bool jsonNumber(JsonReader *reader, float *out);
bool jsonBool(JsonReader *reader, bool *out);
bool jsonSkipValue(JsonReader *reader);

#endif
