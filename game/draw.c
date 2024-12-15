#include "game.h"
#include <stdio.h>


static char CHARS_X[4][4] = {
    {'\\', ' ', ' ', '/'},
    {' ', '\\', '/', ' '},
    {' ', '/', '\\', ' '},
    {'/', ' ', ' ', '\\'}
};

static char CHARS_O[4][4] = {
    {' ', '-', '-', ' '},
    {'|', ' ', ' ', '|'},
    {'|', ' ', ' ', '|'},
    {' ', '-', '-', ' '}
};



void clearCurrentLine() {
    printf("\x1b[2K\r");  // Clear the line and move the caret to the beginning
}

void moveCaretUp() {
    printf("\x1b[A");  // ANSI escape code for moving the caret up one line
}

void addLine() {
    printf("\n");  // ANSI escape code for moving the caret down one line
}

void drawGame(game_t* game_p, int sx, int sy) {
    for (int y = 0; y < 3; y++) {
        if (y > 0) {
            clearCurrentLine();
            printf("------------+------------+------------");
            addLine();
            fflush(stdout);
        }

        for (int r = 0; r < 4; r++) {
            clearCurrentLine();
            for (int x = 0; x < 3; x++) {
                if (x > 0) {
                    printf("|");
                }

                int cell = CELL_AT(game_p, x, y);
                if (sx == x && sy == y) {
                    printf(">>  ");
                } else {
                    printf("    ");
                }
                if (cell == CELL_EMPTY) {
                    printf("    ");
                } else {
                    char* pixel_ptr = cell == CELL_X ? &CHARS_X[r][0] : &CHARS_O[r][0];
                    for (int i = 0; i < 4; i++) {
                        printf("%c", pixel_ptr[i]);
                    }
                }
                if (sx == x && sy == y) {
                    printf("  <<");
                } else {
                    printf("    ");
                }
            }
            addLine();
        }
    }
}

void returnGame() {
    for (int i = 0; i < 14; i++) {
        moveCaretUp();
    }
}