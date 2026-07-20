#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "raylib.h"

#include "../input/keyboard.h"

void uiDrawCenteredText(const char *text, Rectangle bounds, int fontSize,
                        int minFontSize, Color color);
void uiDrawCard(const char *text, Rectangle bounds, bool selected, bool hovered,
                bool enabled, int fontSize);
bool uiWasClicked(const KeyboardState *keyboard, Rectangle bounds);

#endif
