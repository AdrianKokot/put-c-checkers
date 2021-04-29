
#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.h>
#include "./board.h"

/**
 * @brief Create sfRenderwindow
 *
 * @param size size of the window (squre)
 * @return sfRenderWindow*
 */
sfRenderWindow *engine_createWindow(int size);

/**
 * @brief Create sprite with texture from given path
 *
 * @param texturePath path to the texture
 * @return sfSprite*
 */
sfSprite *engine_createSprite(char *texturePath);

/**
 * @brief Create sprite with given texture
 *
 * @param spriteTexture sfTexture pointer
 *                      that will be attached to sprite
 * @return sfSprite*
 */
sfSprite *engine_createSpriteFromTexture(sfTexture *spriteTexture);

/**
 * @brief Check sfRenderWindow events
 *
 * @param board
 */
void engine_checkEvents(Board *board);

#endif