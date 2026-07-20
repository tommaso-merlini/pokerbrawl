#include "../menu_internal.h"

#include "../../../ui/ui.h"

void menuMapsUpdate(GameState *game, const KeyboardState *keyboard,
                    MenuLayout layout) {
  for (int i = 0; i < game->availableMaps.count; i++) {
    Rectangle card = menuMapCard(layout.content, i);

    if (CheckCollisionPointRec(keyboard->pointer, layout.content) &&
        uiWasClicked(keyboard, card)) {
      game->selectedMapIndex = i;
    }
  }
}

void menuMapsDraw(const GameState *game, MenuLayout layout) {
  if (game->availableMaps.count <= 0) {
    uiDrawCenteredText(game->mapLoadError[0] != '\0' ? game->mapLoadError
                                                     : "Nessuna mappa trovata",
                       layout.content, 22, 12, RED);
    return;
  }

  for (int i = 0; i < game->availableMaps.count; i++) {
    Rectangle card = menuMapCard(layout.content, i);
    bool selected = i == game->selectedMapIndex;
    bool hovered = CheckCollisionPointRec(game->ui.pointer, layout.content) &&
                   CheckCollisionPointRec(game->ui.pointer, card);

    uiDrawCard(game->availableMaps.maps[i].name, card, selected, hovered, true,
               20);
  }
}
