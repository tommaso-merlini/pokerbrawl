#include "../menu_internal.h"

#include "../../../ui/ui.h"

#include <stdio.h>

void menuCharactersUpdate(GameState *game, const InputState *input,
                          MenuLayout layout) {
  if (game->characterCount <= 0) {
    return;
  }

  for (int playerIndex = 0; playerIndex < getActivePlayerCount(game);
       playerIndex++) {
    for (int i = 0; i < game->characterCount; i++) {
      Rectangle card = menuCharacterCard(layout.content, playerIndex, i,
                                         game->characterCount);
      if (uiWasClicked(input, card)) {
        setPlayerCharacter(game, playerIndex, i);
      }
    }
  }
}

void menuCharactersDraw(const GameState *game, MenuLayout layout) {
  if (game->characterCount <= 0) {
    uiDrawCenteredText("Nessun personaggio trovato", layout.content, 22, 12,
                       RED);
    return;
  }

  for (int playerIndex = 0; playerIndex < getActivePlayerCount(game);
       playerIndex++) {
    Rectangle section = menuPlayerSection(layout.content, playerIndex);
    char label[32];

    snprintf(label, sizeof(label), "Player %d", playerIndex + 1);
    DrawText(label, (int)section.x, (int)section.y, 24, BLACK);

    for (int i = 0; i < game->characterCount; i++) {
      Rectangle card = menuCharacterCard(layout.content, playerIndex, i,
                                         game->characterCount);
      bool selected = playerHasCharacter(game, playerIndex, i);
      bool hovered = CheckCollisionPointRec(game->ui.pointer, card);

      uiDrawCard(game->availableCharacters[i].name, card, selected, hovered,
                 true, 20);
    }
  }
}
