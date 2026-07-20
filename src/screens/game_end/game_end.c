#include "game_end.h"

#include "../../ui/ui.h"

#include <stdio.h>

typedef struct GameEndLayout {
  Rectangle panel;
  Rectangle eyebrow;
  Rectangle result;
  Rectangle winner;
  Rectangle button;
} GameEndLayout;

static float clampf(float value, float minimum, float maximum) {
  if (value < minimum) {
    return minimum;
  }
  return value > maximum ? maximum : value;
}

static GameEndLayout gameEndLayout(int screenWidth, int screenHeight) {
  float width = clampf((float)screenWidth - 64.0f, 300.0f, 680.0f);
  float height = clampf((float)screenHeight - 64.0f, 300.0f, 430.0f);
  Rectangle panel = {(screenWidth - width) * 0.5f,
                     (screenHeight - height) * 0.5f, width, height};
  float contentWidth = width - 72.0f;

  return (GameEndLayout){
      .panel = panel,
      .eyebrow = {panel.x + 36.0f, panel.y + 42.0f, contentWidth, 28.0f},
      .result = {panel.x + 36.0f, panel.y + 86.0f, contentWidth, 78.0f},
      .winner = {panel.x + 36.0f, panel.y + 170.0f, contentWidth, 54.0f},
      .button = {panel.x + (width - 230.0f) * 0.5f,
                 panel.y + height - 82.0f, 230.0f, 48.0f},
  };
}

void gameEndScreenUpdate(GameState *game, const KeyboardState *keyboard) {
  GameEndLayout layout = gameEndLayout(GetScreenWidth(), GetScreenHeight());
  if (keyboard->confirmPressed || keyboard->backPressed ||
      uiWasClicked(keyboard, layout.button)) {
    returnToMenu(game);
  }
}

void gameEndScreenDraw(const GameState *game, int screenWidth,
                       int screenHeight) {
  GameEndLayout layout = gameEndLayout(screenWidth, screenHeight);
  bool hasWinner = game->winnerPlayerIndex >= 0 &&
                   game->winnerPlayerIndex < game->playerCount;
  char winnerLabel[128] = "";

  ClearBackground((Color){24, 28, 38, 255});
  DrawRectangleRec(layout.panel, (Color){248, 245, 235, 255});
  DrawRectangleLinesEx(layout.panel, 3.0f, BLACK);
  DrawRectangle((int)layout.panel.x, (int)layout.panel.y,
                (int)layout.panel.width, 10, (Color){35, 115, 230, 255});

  uiDrawCenteredText("PARTITA TERMINATA", layout.eyebrow, 20, 14, DARKGRAY);
  uiDrawCenteredText(hasWinner ? "VINCITORE!" : "PAREGGIO!", layout.result,
                     56, 30, hasWinner ? (Color){35, 115, 230, 255} : BLACK);

  if (hasWinner) {
    const Player *winner = &game->players[game->winnerPlayerIndex];
    snprintf(winnerLabel, sizeof(winnerLabel), "P%d  %s",
             game->winnerPlayerIndex + 1,
             winner->character.name[0] != '\0' ? winner->character.name
                                                : "Unknown");
    uiDrawCenteredText(winnerLabel, layout.winner, 32, 18, BLACK);
  } else {
    uiDrawCenteredText("Nessun giocatore rimasto", layout.winner, 24, 16,
                       DARKGRAY);
  }

  bool hovered = CheckCollisionPointRec(game->ui.pointer, layout.button);
  uiDrawCard("MENU PRINCIPALE", layout.button, false, hovered, true, 20);
}
