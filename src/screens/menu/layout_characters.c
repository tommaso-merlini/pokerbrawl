#include "menu_internal.h"

Rectangle menuPlayerSection(Rectangle content, int playerIndex) {
  float gap = 18.0f;
  float width = (content.width - gap) * 0.5f;
  return (Rectangle){content.x + (float)playerIndex * (width + gap), content.y,
                     width, content.height};
}

static Rectangle characterGrid(Rectangle section) {
  return (Rectangle){section.x, section.y + 34.0f, section.width,
                     section.height - 34.0f};
}

Rectangle menuCharacterCard(Rectangle content, int playerIndex,
                            int characterIndex, int characterCount) {
  Rectangle section = menuPlayerSection(content, playerIndex);
  Rectangle grid = characterGrid(section);
  int columns = section.width >= 230.0f ? 2 : 1;
  int rows = (characterCount + columns - 1) / columns;
  float gap = 10.0f;
  float height =
      rows > 0 ? (grid.height - (float)(rows - 1) * gap) / (float)rows : 44.0f;
  height = height > 56.0f ? 56.0f : height;
  height = height < 28.0f ? 28.0f : height;

  float width = (grid.width - (float)(columns - 1) * gap) / (float)columns;
  int row = characterIndex / columns;
  int column = characterIndex % columns;
  return (Rectangle){grid.x + (float)column * (width + gap),
                     grid.y + (float)row * (height + gap), width, height};
}
