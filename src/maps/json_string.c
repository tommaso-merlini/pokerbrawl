#include "json_reader.h"

#include <ctype.h>

bool jsonString(JsonReader *reader, char *out, int outSize) {
  jsonSkipWhitespace(reader);
  if (outSize <= 0) {
    return jsonFail(reader, "String output buffer is empty");
  }
  if (*reader->at != '"') {
    return jsonFail(reader, "Expected string");
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
            return jsonFail(reader, "Invalid unicode escape");
          }
        }
        reader->at += 4;
        value = '?';
        break;
      default:
        return jsonFail(reader, "Invalid string escape");
      }
    }

    if (length < outSize - 1) {
      out[length++] = value;
    }
  }

  if (*reader->at != '"') {
    return jsonFail(reader, "Unterminated string");
  }
  reader->at++;
  out[length] = '\0';
  return true;
}
