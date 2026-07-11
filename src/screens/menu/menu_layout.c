#include "menu_internal.h"

#include "../../game_state.h"

MenuLayout menuGetLayout(int currentWidth, int currentHeight) {
  float panelWidth = (float)currentWidth - 80.0f;
  float panelHeight = (float)currentHeight - 90.0f;

  if (panelWidth > 900.0f) {
    panelWidth = 900.0f;
  }
  if (panelWidth < 520.0f) {
    panelWidth = (float)currentWidth - 32.0f;
  }
  if (panelWidth < 300.0f) {
    panelWidth = 300.0f;
  }

  if (panelHeight > 680.0f) {
    panelHeight = 680.0f;
  }
  if (panelHeight < 340.0f) {
    panelHeight = (float)currentHeight - 32.0f;
  }
  if (panelHeight < 260.0f) {
    panelHeight = 260.0f;
  }

  MenuLayout layout = {0};
  layout.menuWindow =
      (Rectangle){(float)currentWidth * 0.5f - panelWidth * 0.5f,
                  (float)currentHeight * 0.5f - panelHeight * 0.5f,
                  panelWidth, panelHeight};

  float buttonWidth = (layout.menuWindow.width - 104.0f) * 0.5f;
  if (buttonWidth > 150.0f) {
    buttonWidth = 150.0f;
  }
  if (buttonWidth < 104.0f) {
    buttonWidth = 104.0f;
  }

  layout.content =
      (Rectangle){layout.menuWindow.x + 36.0f, layout.menuWindow.y + 112.0f,
                  layout.menuWindow.width - 72.0f,
                  layout.menuWindow.height - 194.0f};
  if (layout.content.height < 120.0f) {
    layout.content.height = 120.0f;
  }

  layout.backButton =
      (Rectangle){layout.menuWindow.x + 36.0f,
                  layout.menuWindow.y + layout.menuWindow.height - 58.0f,
                  buttonWidth, 42.0f};
  layout.nextButton =
      (Rectangle){layout.menuWindow.x + layout.menuWindow.width - 36.0f -
                      buttonWidth,
                  layout.backButton.y, buttonWidth, layout.backButton.height};

  return layout;
}

Rectangle menuModeCard(Rectangle content) {
  float cardWidth = content.width;
  float cardHeight = 82.0f;

  if (cardWidth > 360.0f) {
    cardWidth = 360.0f;
  }
  if (content.height < cardHeight) {
    cardHeight = content.height;
  }

  return (Rectangle){content.x + content.width * 0.5f - cardWidth * 0.5f,
                     content.y + content.height * 0.5f - cardHeight * 0.5f,
                     cardWidth, cardHeight};
}

Rectangle menuPlayerSection(Rectangle content, int playerIndex) {
  float gap = 18.0f;
  float width = (content.width - gap) * 0.5f;

  return (Rectangle){content.x + (float)playerIndex * (width + gap), content.y,
                     width, content.height};
}

static Rectangle charactergrid(Rectangle section) {
  return (Rectangle){section.x, section.y + 34.0f, section.width,
                     section.height - 34.0f};
}

static int charactercolumns(Rectangle section) {
  return section.width >= 230.0f ? 2 : 1;
}

Rectangle menuCharacterCard(Rectangle content, int playerIndex,
                            int characterIndex) {
  Rectangle section = menuPlayerSection(content, playerIndex);
  Rectangle grid = charactergrid(section);
  int columns = charactercolumns(section);
  int rows = (gGame.characterCount + columns - 1) / columns;
  float gap = 10.0f;
  float cardHeight =
      rows > 0 ? (grid.height - (float)(rows - 1) * gap) / (float)rows : 44.0f;

  if (cardHeight > 56.0f) {
    cardHeight = 56.0f;
  }
  if (cardHeight < 28.0f) {
    cardHeight = 28.0f;
  }

  float cardWidth = (grid.width - (float)(columns - 1) * gap) / (float)columns;
  int row = characterIndex / columns;
  int column = characterIndex % columns;

  return (Rectangle){grid.x + (float)column * (cardWidth + gap),
                     grid.y + (float)row * (cardHeight + gap), cardWidth,
                     cardHeight};
}

static int mapcolumns(Rectangle content) {
  if (content.width >= 760.0f) {
    return 4;
  }
  if (content.width >= 560.0f) {
    return 3;
  }

  return 2;
}

static float mapcardheight(Rectangle content) {
  return content.height < 190.0f ? 58.0f : 74.0f;
}

Rectangle menuMapCard(Rectangle content, int mapIndex) {
  int columns = mapcolumns(content);
  float gap = 12.0f;
  float cardHeight = mapcardheight(content);
  float cardWidth =
      (content.width - (float)(columns - 1) * gap) / (float)columns;
  int row = mapIndex / columns;
  int column = mapIndex % columns;

  return (Rectangle){content.x + (float)column * (cardWidth + gap),
                     content.y + (float)row * (cardHeight + gap),
                     cardWidth, cardHeight};
}
