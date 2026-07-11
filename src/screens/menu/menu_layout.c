#include "menu_internal.h"

static float panelSize(float available, float margin, float maximum,
                       float compactThreshold, float compactMargin,
                       float minimum) {
  float size = available - margin;
  if (size > maximum) {
    size = maximum;
  }
  if (size < compactThreshold) {
    size = available - compactMargin;
  }
  return size < minimum ? minimum : size;
}

MenuLayout menuGetLayout(int screenWidth, int screenHeight) {
  float width =
      panelSize((float)screenWidth, 80.0f, 900.0f, 520.0f, 32.0f, 300.0f);
  float height =
      panelSize((float)screenHeight, 90.0f, 680.0f, 340.0f, 32.0f, 260.0f);
  MenuLayout layout = {0};

  layout.menuWindow =
      (Rectangle){(float)screenWidth * 0.5f - width * 0.5f,
                  (float)screenHeight * 0.5f - height * 0.5f, width, height};

  float buttonWidth = (width - 104.0f) * 0.5f;
  buttonWidth = buttonWidth > 150.0f ? 150.0f : buttonWidth;
  buttonWidth = buttonWidth < 104.0f ? 104.0f : buttonWidth;

  layout.content =
      (Rectangle){layout.menuWindow.x + 36.0f, layout.menuWindow.y + 112.0f,
                  width - 72.0f, height - 194.0f};
  if (layout.content.height < 120.0f) {
    layout.content.height = 120.0f;
  }

  layout.backButton =
      (Rectangle){layout.menuWindow.x + 36.0f,
                  layout.menuWindow.y + height - 58.0f, buttonWidth, 42.0f};
  layout.nextButton =
      (Rectangle){layout.menuWindow.x + width - 36.0f - buttonWidth,
                  layout.backButton.y, buttonWidth, 42.0f};
  return layout;
}

Rectangle menuModeCard(Rectangle content) {
  float width = content.width > 360.0f ? 360.0f : content.width;
  float height = content.height < 82.0f ? content.height : 82.0f;
  return (Rectangle){content.x + (content.width - width) * 0.5f,
                     content.y + (content.height - height) * 0.5f, width,
                     height};
}
