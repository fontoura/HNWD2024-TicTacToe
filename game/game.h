#ifndef __GAME_H_DEFINED__
#define __GAME_H_DEFINED__

typedef struct {
    int grid[9];
} game_t;

#define CELL_EMPTY (0)
#define CELL_X (1)
#define CELL_O (2)

#define CELL_AT(game_p, x, y) ((game_p)->grid[(y) * 3 + (x)])

void runGame();
int gameLoop(game_t* game);
int checkWinner(game_t* game_p);
int allCellsFilled(game_t* game_p);
void playAsComputer(game_t* game_p);

#endif
