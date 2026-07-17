#include "../menu_internal.h"

#include "../../../ui/ui.h"

#include <stdio.h>

static const Color PLAYER_COLORS[MAX_PLAYERS] = {
    {30, 110, 245, 255},
    {235, 55, 65, 255},
    {45, 185, 90, 255},
    {245, 160, 35, 255},
};

static void drawCharacterPortrait(const PlayerSpriteAssets *spriteAssets,
                                  CharacterId character, Rectangle bounds) {
  const Texture2D *texture =
      getPlayerSpritePose(spriteAssets, character, PLAYER_SPRITE_POSE_IDLE);
  if (texture == NULL || !IsTextureValid(*texture)) {
    return;
  }

  float scaleX = bounds.width / (float)texture->width;
  float scaleY = bounds.height / (float)texture->height;
  float scale = scaleX < scaleY ? scaleX : scaleY;
  Rectangle destination = {
      bounds.x + (bounds.width - (float)texture->width * scale) * 0.5f,
      bounds.y + (bounds.height - (float)texture->height * scale) * 0.5f,
      (float)texture->width * scale,
      (float)texture->height * scale,
  };
  Rectangle source = {0.0f, 0.0f, (float)texture->width,
                      (float)texture->height};
  DrawTexturePro(*texture, source, destination, (Vector2){0}, 0.0f, WHITE);
}

static void drawPlayerBadge(int playerIndex, int badgeIndex, Rectangle card) {
  float badgeWidth = card.height >= 60.0f ? 38.0f : 30.0f;
  float badgeHeight = card.height >= 60.0f ? 26.0f : 20.0f;
  float badgeInset = card.height >= 60.0f ? 7.0f : 4.0f;
  Rectangle badge = {card.x + badgeInset +
                         (float)badgeIndex * (badgeWidth + 4.0f),
                     card.y + badgeInset, badgeWidth, badgeHeight};
  char label[16];

  snprintf(label, sizeof(label), "P%d", playerIndex + 1);
  DrawRectangleRec(badge, PLAYER_COLORS[playerIndex]);
  DrawRectangleLinesEx(badge, 2.0f, BLACK);
  uiDrawCenteredText(label, badge, card.height >= 60.0f ? 17 : 13, 10,
                     RAYWHITE);
}

static void drawCharacterCard(const GameState *game,
                              const PlayerSpriteAssets *spriteAssets,
                              int characterIndex, Rectangle card) {
  bool hovered = CheckCollisionPointRec(game->ui.pointer, card);
  Color fill =
      hovered ? (Color){245, 245, 245, 255} : (Color){218, 218, 218, 255};
  float footerHeight =
      card.height >= 100.0f ? 34.0f : (card.height >= 60.0f ? 26.0f : 18.0f);
  float padding = card.height >= 60.0f ? 8.0f : 3.0f;
  Rectangle portrait = {card.x + padding, card.y + padding,
                        card.width - padding * 2.0f,
                        card.height - footerHeight - padding * 1.5f};
  Rectangle footer = {card.x, card.y + card.height - footerHeight, card.width,
                      footerHeight};

  DrawRectangleRec(card, fill);
  DrawRectangleRec(footer, (Color){35, 35, 42, 255});
  drawCharacterPortrait(spriteAssets,
                        game->availableCharacters[characterIndex].id, portrait);
  uiDrawCenteredText(game->availableCharacters[characterIndex].name, footer,
                     card.height >= 60.0f ? 20 : 13, 10, RAYWHITE);
  DrawRectangleLinesEx(card, 2.0f, BLACK);

  int badgeIndex = 0;
  for (int playerIndex = 0; playerIndex < getActivePlayerCount(game);
       playerIndex++) {
    if (!playerHasCharacter(game, playerIndex, characterIndex)) {
      continue;
    }

    Rectangle outline = {card.x + (float)badgeIndex * 4.0f,
                         card.y + (float)badgeIndex * 4.0f,
                         card.width - (float)badgeIndex * 8.0f,
                         card.height - (float)badgeIndex * 8.0f};
    DrawRectangleLinesEx(outline, 4.0f, PLAYER_COLORS[playerIndex]);
    drawPlayerBadge(playerIndex, badgeIndex, card);
    badgeIndex++;
  }
}

void menuCharactersUpdate(GameState *game, const InputState *input,
                          MenuLayout layout) {
  if (game->characterCount <= 0) {
    return;
  }

  for (int i = 0; i < game->characterCount; i++) {
    Rectangle card = menuCharacterCard(layout.content, i, game->characterCount);
    if (uiWasClicked(input, card)) {
      // Mouse and keyboard own P1; each connected gamepad owns the player with
      // the same connection-order index.
      setPlayerCharacter(game, 0, i);
    }
  }
}

void menuCharactersDraw(const GameState *game,
                        const PlayerSpriteAssets *spriteAssets,
                        MenuLayout layout) {
  if (game->characterCount <= 0) {
    uiDrawCenteredText("Nessun personaggio trovato", layout.content, 22, 12,
                       RED);
    return;
  }

  for (int i = 0; i < game->characterCount; i++) {
    Rectangle card = menuCharacterCard(layout.content, i, game->characterCount);
    drawCharacterCard(game, spriteAssets, i, card);
  }
}
