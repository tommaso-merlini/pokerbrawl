#include "../menu_internal.h"

#include "../../../game_state.h"
#include "../../../ui.h"

#include <stdio.h>

void menuCharactersUpdate(MenuLayout layout) {
  if (gGame.characterCount <= 0) {
    return;
  }

  for (int playerIndex = 0; playerIndex < getActivePlayerCount();
       playerIndex++) {
    for (int i = 0; i < gGame.characterCount; i++) {
      Rectangle card = menuCharacterCard(layout.content, playerIndex, i);
      if (uiWasClicked(card)) {
        setPlayerCharacter(playerIndex, i);
      }
    }
  }
}

void menuCharactersDraw(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();

  if (gGame.characterCount <= 0) {
    uiDrawCenteredText("Nessun personaggio trovato", layout.content, 22, 12,
                       RED);
    return;
  }

  for (int playerIndex = 0; playerIndex < getActivePlayerCount();
       playerIndex++) {
    Rectangle section = menuPlayerSection(layout.content, playerIndex);
    char label[32];

    snprintf(label, sizeof(label), "Player %d", playerIndex + 1);
    DrawText(label, (int)section.x, (int)section.y, 24, BLACK);

    for (int i = 0; i < gGame.characterCount; i++) {
      Rectangle card = menuCharacterCard(layout.content, playerIndex, i);
      bool selected = playerHasCharacter(playerIndex, i);
      bool hovered = CheckCollisionPointRec(mouse, card);

      uiDrawCard(gGame.availableCharacters[i].name, card, selected, hovered,
                 true, 20);
    }
  }
}
