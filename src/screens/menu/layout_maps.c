#include "menu_internal.h"

int menuMapColumns(Rectangle content) {
  if (content.width >= 760.0f) {
    return 4;
  }
  return content.width >= 560.0f ? 3 : 2;
}

Rectangle menuMapCard(Rectangle content, int mapIndex) {
  int columns = menuMapColumns(content);
  float gap = 12.0f;
  float height = content.height < 190.0f ? 58.0f : 74.0f;
  float width = (content.width - (float)(columns - 1) * gap) / (float)columns;
  int row = mapIndex / columns;
  int column = mapIndex % columns;
  return (Rectangle){content.x + (float)column * (width + gap),
                     content.y + (float)row * (height + gap), width, height};
}
