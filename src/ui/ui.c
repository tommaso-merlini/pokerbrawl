#include "ui.h"

static int fitTextSize(const char *text, int maxWidth, int fontSize,
                       int minFontSize) {
  while (fontSize > minFontSize && MeasureText(text, fontSize) > maxWidth) {
    fontSize -= 2;
  }

  return fontSize;
}

void uiDrawCenteredText(const char *text, Rectangle bounds, int fontSize,
                        int minFontSize, Color color) {
  int size = fitTextSize(text, (int)bounds.width - 24, fontSize, minFontSize);
  int textWidth = MeasureText(text, size);

  DrawText(text, (int)(bounds.x + bounds.width * 0.5f - textWidth * 0.5f),
           (int)(bounds.y + bounds.height * 0.5f - size * 0.5f), size, color);
}

void uiDrawCard(const char *text, Rectangle bounds, bool selected, bool hovered,
                bool enabled, int fontSize) {
  Color fill =
      enabled ? (Color){210, 210, 210, 255} : (Color){170, 170, 170, 255};
  Color outline = enabled ? BLACK : DARKGRAY;
  Color textColor = enabled ? BLACK : DARKGRAY;

  if (selected && enabled) {
    fill = (Color){35, 115, 230, 255};
    outline = (Color){12, 45, 120, 255};
    textColor = RAYWHITE;
  } else if (hovered && enabled) {
    fill = (Color){235, 235, 235, 255};
  }

  DrawRectangleRec(bounds, fill);
  DrawRectangleLinesEx(bounds, 2.0f, outline);
  uiDrawCenteredText(text, bounds, fontSize, 12, textColor);
}

bool uiWasClicked(const InputState *input, Rectangle bounds) {
  return inputClicked(input, bounds);
}
