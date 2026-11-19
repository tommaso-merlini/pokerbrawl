#include "game_screen.h"

#include "../../ui/ui.h"

static const char *PAUSE_LABELS[PAUSE_OPTION_COUNT] = {
    [PAUSE_OPTION_RESUME] = "RIPRENDI",
    [PAUSE_OPTION_MENU] = "TORNA AL MENU",
};

typedef struct PauseLayout {
  Rectangle panel;
  Rectangle title;
  Rectangle buttons[PAUSE_OPTION_COUNT];
} PauseLayout;

static PauseLayout pauseLayout(int screenWidth, int screenHeight) {
  float width = 360.0f;
  float buttonHeight = 52.0f;
  float gap = 14.0f;
  float height = 96.0f + PAUSE_OPTION_COUNT * (buttonHeight + gap) + 20.0f;
  Rectangle panel = {(screenWidth - width) * 0.5f,
                     (screenHeight - height) * 0.5f, width, height};

  PauseLayout layout = {
      .panel = panel,
      .title = {panel.x, panel.y + 24.0f, width, 52.0f},
  };
  for (int i = 0; i < PAUSE_OPTION_COUNT; i++) {
    layout.buttons[i] =
        (Rectangle){panel.x + 40.0f, panel.y + 96.0f + i * (buttonHeight + gap),
                    width - 80.0f, buttonHeight};
  }
  return layout;
}

static void choose(GameState *game, PauseOption option) {
  switch (option) {
  case PAUSE_OPTION_RESUME:
    game->pause.active = false;
    break;
  case PAUSE_OPTION_MENU:
    returnToMenu(game);
    break;
  case PAUSE_OPTION_COUNT:
    break;
  }
}

static void movePauseSelection(GameState *game, int delta) {
  int next = ((int)game->pause.selected + delta + PAUSE_OPTION_COUNT) %
             PAUSE_OPTION_COUNT;
  game->pause.selected = (PauseOption)next;
}

static void receivePauseNavigation(void *context, int playerIndex,
                                   UiNavigationCommand command) {
  (void)playerIndex;
  GameState *game = context;
  if (!game->pause.active) {
    return; // an earlier command this frame already closed the pause
  }

  switch (command) {
  case UI_NAVIGATION_UP:
    movePauseSelection(game, -1);
    break;
  case UI_NAVIGATION_DOWN:
    movePauseSelection(game, 1);
    break;
  case UI_NAVIGATION_CONFIRM:
    choose(game, game->pause.selected);
    break;
  case UI_NAVIGATION_BACK:
    game->pause.active = false;
    break;
  case UI_NAVIGATION_LEFT:
  case UI_NAVIGATION_RIGHT:
    break;
  }
}

// Returns true while the match is paused (gameplay must not advance).
bool gamePauseUpdate(GameState *game, const KeyboardState *keyboard,
                     const ControllerRegistry *controllers) {
  bool togglePressed =
      keyboard->backPressed || controllerRegistryPausePressed(controllers);

  if (!game->pause.active) {
    if (togglePressed) {
      game->pause = (PauseState){.active = true,
                                 .selected = PAUSE_OPTION_RESUME};
      return true;
    }
    return false;
  }

  if (togglePressed) {
    game->pause.active = false;
    return true;
  }

  PauseLayout layout = pauseLayout(GetScreenWidth(), GetScreenHeight());
  for (int i = 0; i < PAUSE_OPTION_COUNT; i++) {
    if (CheckCollisionPointRec(keyboard->pointer, layout.buttons[i])) {
      game->pause.selected = (PauseOption)i;
    }
    if (uiWasClicked(keyboard, layout.buttons[i])) {
      choose(game, (PauseOption)i);
      return true;
    }
  }

  UiNavigationTarget target = {.context = game,
                               .send = receivePauseNavigation};
  // Keyboard: arrows / WASD to move, Enter to confirm. Esc is the toggle above.
  if (keyboard->navigateUpPressed) {
    uiSendNavigation(&target, 0, UI_NAVIGATION_UP);
  }
  if (keyboard->navigateDownPressed) {
    uiSendNavigation(&target, 0, UI_NAVIGATION_DOWN);
  }
  if (keyboard->confirmPressed) {
    uiSendNavigation(&target, 0, UI_NAVIGATION_CONFIRM);
  }
  controllerRegistrySendNavigation(controllers, &target);
  return true;
}

void gamePauseDraw(const GameState *game, int screenWidth, int screenHeight) {
  if (!game->pause.active || game->screen != SCREEN_GAME) {
    return;
  }

  PauseLayout layout = pauseLayout(screenWidth, screenHeight);
  DrawRectangle(0, 0, screenWidth, screenHeight, (Color){8, 10, 18, 170});
  DrawRectangleRec(layout.panel, (Color){248, 245, 235, 255});
  DrawRectangleLinesEx(layout.panel, 3.0f, BLACK);
  DrawRectangle((int)layout.panel.x, (int)layout.panel.y,
                (int)layout.panel.width, 10, (Color){35, 115, 230, 255});
  uiDrawCenteredText("PAUSA", layout.title, 40, 24, BLACK);

  for (int i = 0; i < PAUSE_OPTION_COUNT; i++) {
    bool hovered = CheckCollisionPointRec(game->ui.pointer, layout.buttons[i]);
    uiDrawCard(PAUSE_LABELS[i], layout.buttons[i],
               game->pause.selected == (PauseOption)i, hovered, true, 22);
  }
}
