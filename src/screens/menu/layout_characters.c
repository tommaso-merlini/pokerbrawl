#include "menu_internal.h"

int menuCharacterColumns(Rectangle content, int characterCount) {
  int columns = 2;
  if (content.width >= 700.0f) {
    columns = 5;
  } else if (content.width >= 480.0f) {
    columns = 3;
  }
  return characterCount < columns ? characterCount : columns;
}

Rectangle menuCharacterCard(Rectangle content, int characterIndex,
                            int characterCount) {
  int columns = menuCharacterColumns(content, characterCount);
  int rows = (characterCount + columns - 1) / columns;
  float gap = 14.0f;
  float height = rows > 0
                     ? (content.height - (float)(rows - 1) * gap) / (float)rows
                     : 80.0f;
  height = height > 210.0f ? 210.0f : height;
  height = height < 28.0f ? 28.0f : height;

  float width = (content.width - (float)(columns - 1) * gap) / (float)columns;
  int row = characterIndex / columns;
  int column = characterIndex % columns;
  float usedWidth = (float)columns * width + (float)(columns - 1) * gap;
  float usedHeight = (float)rows * height + (float)(rows - 1) * gap;
  return (Rectangle){content.x + (content.width - usedWidth) * 0.5f +
                         (float)column * (width + gap),
                     content.y + (content.height - usedHeight) * 0.5f +
                         (float)row * (height + gap),
                     width, height};
}
