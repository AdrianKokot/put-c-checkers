#include "../headers/pawn.h"

Pawn *pawn_create(int iPosX, int iPosY, Player *player)
{
  Pawn *pawn = malloc(sizeof(Pawn));

  pawn->player = player;
  pawn->position = malloc(sizeof(BoardPosition));
  pawn->position->x = iPosX;
  pawn->position->y = iPosY;
  pawn->sprite = sfSprite_create();
  pawn->availableBeats = (Pawn **)malloc(sizeof(Pawn *) * 4);

  pawn_setType(pawn, Standard);

  return pawn;
}

void pawn_setTexture(Pawn *pawn, const sfTexture *texture)
{
  sfSprite_setTexture(pawn->sprite, texture, sfTrue);
}

void pawn_setType(Pawn *pawn, PawnType type)
{
  pawn->pawnType = type;

  pawn_setTexture(pawn, pawn->player->textures[pawn->pawnType]);
}

// FIXME some bugs with beating as black
BoardPosition pawn_checkMoveAvailableness(Pawn *pawn, int x, int y, int **pawnsOnBoard, int boardSize, int yDiff)
{
  if (x >= 0 && x < boardSize && y >= 0 && y < boardSize)
  {
    if (pawnsOnBoard[y][x] == 0)
    {
      BoardPosition position = {x, y};
      return position;
    }
    else if (pawnsOnBoard[y][x] == -1)
    {
      int idx = pawn->position->x < x ? (pawn->position->y > y ? 0 : 1) : (pawn->position->y > y ? 3 : 2);

      pawn->availableBeats[idx] = pawn_findByPos(pawn->player->board, x, y);

      return pawn_checkMoveAvailableness(pawn, pawn->position->x > x ? x - 1 : x + 1, y + yDiff, pawnsOnBoard, boardSize, yDiff);
    }
  }
  BoardPosition position = {-1, -1};
  return position;
}

// FIXME probably to re-think
BoardPosition *pawn_getAvailableMoves(Pawn *pawn)
{
  int boardSize = pawn->player->board->boardSize;
  int **pawnsOnBoard = (int **)malloc(sizeof(int *) * boardSize);

  int maxAvailableMoves = boardSize;
  BoardPosition *availableMoves = malloc(sizeof(BoardPosition) * maxAvailableMoves);
  int filledAvailableMovesCount = 0;

  for (int i = 0; i < boardSize; i++)
  {
    pawnsOnBoard[i] = (int *)malloc(sizeof(int) * boardSize);
    for (int j = 0; j < boardSize; j++)
    {
      pawnsOnBoard[i][j] = 0;
    }
  }

  Board *board = pawn->player->board;

  for (int x = 0; x < board->playerCount; x++)
  {
    for (int i = 0; i < board->players[x]->iPawnCount; i++)
    {
      pawnsOnBoard[board->players[x]->pawns[i]->position->y][board->players[x]->pawns[i]->position->x] = board->players[x]->bIsActive ? 1 : -1;
    }
  }

  int yDiff = pawn->player->board->players[0]->bIsActive ? 1 : -1;

  BoardPosition pos1 = pawn_checkMoveAvailableness(pawn, pawn->position->x + 1, pawn->position->y + yDiff, pawnsOnBoard, boardSize, yDiff);
  if (pos1.x != -1)
  {
    availableMoves[filledAvailableMovesCount] = pos1;
    filledAvailableMovesCount++;
  }

  BoardPosition pos2 = pawn_checkMoveAvailableness(pawn, pawn->position->x - 1, pawn->position->y + yDiff, pawnsOnBoard, boardSize, yDiff);
  if (pos2.x != -1)
  {
    availableMoves[filledAvailableMovesCount] = pos2;
    filledAvailableMovesCount++;
  }

  if (pawn->pawnType == Queen)
  {
    for (int i = 0; i < boardSize; i += 2)
    {
      for (int j = 0; j < boardSize; j += 2)
      {
        BoardPosition pos = pawn_checkMoveAvailableness(pawn, i, j, pawnsOnBoard, boardSize, yDiff);
        if (pos.x != -1)
        {
          availableMoves[filledAvailableMovesCount] = pos2;
          filledAvailableMovesCount++;
        }
      }
    }
  }

  BoardPosition nullPos = {-1, -1};
  while (filledAvailableMovesCount < maxAvailableMoves)
  {
    availableMoves[filledAvailableMovesCount] = nullPos;
    filledAvailableMovesCount++;
  }

  return availableMoves;
}

void pawn_markAvailableMoves(Pawn *pawn)
{
  board_resetTilesTextures(pawn->player->board);
  BoardPosition *availablePositions = pawn_getAvailableMoves(pawn);

  for (int i = 0; availablePositions[i].x != -1; i++)
  {
    board_markTileTexture(pawn->player->board, availablePositions[i].x, availablePositions[i].y);
  }
}

// FIXME bugs with beating as black
void pawn_move(Pawn *pawn, BoardPosition position)
{

  if ((pawn->player->board->players[0]->bIsActive && position.y == 0) || (pawn->player->board->players[0]->bIsActive && position.y == pawn->player->board->boardSize - 1))
  {
    pawn_setType(pawn, Queen);
  }

  int xDiff = abs(pawn->position->x - position.x);
  int yDiff = abs(pawn->position->y - position.y);

  int idx = pawn->position->x < position.x ? (pawn->position->y > position.y ? 0 : 1) : (pawn->position->y > position.y ? 3 : 2);

  if (pawn->availableBeats[idx])
  {
    pawn_remove(pawn->availableBeats[idx]);
  }

  sfVector2f vectorOffset = {
      position.x * pawn->player->board->textureSize + pawn->player->board->boardBorder,
      position.y * pawn->player->board->textureSize + pawn->player->board->boardBorder};

  pawn->position->x = position.x;
  pawn->position->y = position.y;
  sfSprite_setPosition(pawn->sprite, vectorOffset);
}

void pawn_remove(Pawn *pawn)
{
  Pawn *temp = pawn->player->pawns[pawn->player->iPawnCount - 1];
  for (int i = 0; i < pawn->player->iPawnCount; i++)
  {
    if (pawn->player->pawns[i] == pawn)
    {
      pawn->player->pawns[i] = temp;
      pawn->player->pawns[pawn->player->iPawnCount - 1] = pawn;
      pawn->position->x = -1;
      pawn->position->y = -1;
      sfSprite_destroy(pawn->sprite);
      pawn->player->iPawnCount--;
      return;
    }
  }
}

Pawn *pawn_findByPos(Board *board, int x, int y) {
  for (int i = 0; i < 2; i++)
  {
    for(int j = 0; j<board->players[i]->iPawnCount; j++) {
      if(board->players[i]->pawns[j]->position->x == x && board->players[i]->pawns[j]->position->y == y){
        return board->players[i]->pawns[j];
      }
    }
  }
  return board->players[0]->pawns[0];
}