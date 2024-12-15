#ifndef __DRAW_H_DEFINED__
#define __DRAW_H_DEFINED__

#include "game.h"

void drawGame(game_t* game_p, int x, int y);
void returnGame();
void clearCurrentLine();
void moveCaretUp();
void addLine();

#endif
