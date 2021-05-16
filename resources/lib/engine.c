#include "../headers/engine.h"

sfRenderWindow *engine_createWindow(int size)
{
  char *windowTitle = "Warcaby";

  sfVideoMode videoMode = {size, size, 64};
  sfRenderWindow *window = sfRenderWindow_create(videoMode, windowTitle, sfClose, NULL);

  return window;
}

sfSprite *engine_createSprite(char *texturePath)
{
  sfTexture *spriteTexture = sfTexture_createFromFile(texturePath, NULL);
  sfSprite *sprite = sfSprite_create();

  sfSprite_setTexture(sprite, spriteTexture, sfTrue);

  return sprite;
}

sfSprite *engine_createSpriteFromTexture(sfTexture *spriteTexture)
{
  sfSprite *sprite = sfSprite_create();
  sfSprite_setTexture(sprite, spriteTexture, sfTrue);

  return sprite;
}

void engine_checkEvents(Board *board)
{
  sfEvent event;
  while (sfRenderWindow_pollEvent(board->window, &event))
  {
    if (event.type == sfEvtClosed)
    {
      sfRenderWindow_close(board->window);
    }
    if (event.type == sfEvtMouseButtonPressed)
    {
      board_checkPawnSelectionByMouse(board, event.mouseButton.x, event.mouseButton.y);
      board_checkTileSelectionByMouse(board, event.mouseButton.x, event.mouseButton.y);
    }
  }
}

bool engine_checkWinState(Board *board)
{
  if (board_checkWinStatus(board))
  {
    int wonPlayerIndex = board_getWinStatus(board);

    sfText *endText = sfText_create();
    sfText_setString(endText, wonPlayerIndex == 0 ? "Gracz nr 1 wygral" : "Gracz nr 2 wygral");
    sfText_setFont(endText, board->font);
    sfText_setCharacterSize(endText, 50);
    sfText_setColor(endText, sfWhite);

    sfRenderWindow_drawText(board->window, endText, NULL);

    return true;
  }
  return false;
}

void engine_attachBackgroundToBoard(Board *board)
{
  int intWindowSize = board->boardSize * board->textureSize + 2 * board->boardBorder;

  sfColor sfColorBoardBackground = sfColor_fromRGB(78, 52, 46);
  sfSprite *spriteBoardBackground = engine_createSprite("./resources/sprites/board_background.jpg");

  sfVector2f shapeBoardBorderSize = {intWindowSize - 32, intWindowSize - 32};
  sfVector2f shapeBoardBorderPosition = {16, 16};
  sfRectangleShape *shapeBoardBorder = sfRectangleShape_create();

  sfRectangleShape_setFillColor(shapeBoardBorder, sfColorBoardBackground);
  sfRectangleShape_setSize(shapeBoardBorder, shapeBoardBorderSize);
  sfRectangleShape_setPosition(shapeBoardBorder, shapeBoardBorderPosition);

  board->spriteBackground = spriteBoardBackground;
  board->shapeBackground = shapeBoardBorder;
}