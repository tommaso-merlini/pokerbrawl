#ifndef JSON_READER_H
#define JSON_READER_H

#include <stdbool.h>

typedef struct JsonReader {
  const char *at;
  char error[256];
} JsonReader;

void jsoninit(JsonReader *reader, const char *text);
bool jsonfail(JsonReader *reader, const char *message);
void jsonskipws(JsonReader *reader);
bool jsonconsume(JsonReader *reader, char expected);
bool jsonexpect(JsonReader *reader, char expected);
bool jsonstring(JsonReader *reader, char *out, int outSize);
bool jsonnumber(JsonReader *reader, float *out);
bool jsonbool(JsonReader *reader, bool *out);
bool jsonskipvalue(JsonReader *reader);

#endif
