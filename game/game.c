#include "game.h"
#include "draw.h"
#include "setup.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
    #include <windows.h>  // Windows-specific header for Sleep().
    #define sleep(x) Sleep(1000 * (x))  // Redefine sleep for Windows (takes milliseconds).
#else
    #include <unistd.h>  // Unix-like system (Linux/macOS) for sleep().
#endif

void runGame() {
    setupTerminal();

    game_t game;

    int gamesPlayed = 0;
    int scoreX = 0;
    int scoreO = 0;
    while (1) {
        clearCurrentLine();
        printf("Press ENTER to start the game, ESC to exit");

        Action_t action = readAction();
        if (action == ACTION_ESCAPE) {
            break;
        } else if (action != ACTION_ENTER) {
            continue;
        }

        gamesPlayed ++;
        for (int i = 0; i < 9; i++) {
            game.grid[i] = CELL_EMPTY;
        }
        
        clearCurrentLine();
        printf("TicTacToe game is running. Click ESC to give up, ENTER to play!");

        addLine();
        clearCurrentLine();
        addLine();
        clearCurrentLine();
        printf("Player score: %i", scoreX);
        addLine();
        clearCurrentLine();
        printf("Computer score: %i", scoreO);
        addLine();
        clearCurrentLine();
        addLine();
        clearCurrentLine();
        
        int winner = gameLoop(&game);
        if (winner == CELL_X) {
            scoreX++;
        } else if (winner == CELL_O) {
            scoreO++;
        }
        returnGame();
        moveCaretUp();
        moveCaretUp();
        clearCurrentLine();
        if (winner == CELL_X) {
            printf("The player won!");
        } else if (winner == CELL_O) {
            printf("The computer won!");
        } else {
            printf("No one won!");
        }

        moveCaretUp();
        moveCaretUp();
        moveCaretUp();
        clearCurrentLine();
        printf("Player score: %i", scoreX);
        addLine();
        clearCurrentLine();
        printf("Computer score: %i", scoreO);
        moveCaretUp();
        moveCaretUp();
        moveCaretUp();
    }

    if (gamesPlayed > 0) {
        for (int i = 0; i < 20; i ++) {
            addLine();
        }
    }

    restoreTerminal();
}

int gameLoop(game_t* game) {
    int selectedX = 0;
    int selectedY = 0;

    int mustRedraw = 0;
    
    clearCurrentLine();
    printf("It is the player's turn");
    addLine();
    clearCurrentLine();
    addLine();
    drawGame(game, selectedX, selectedY);

    while (1) {
        if (mustRedraw) {
            mustRedraw = 0;
        }
        Action_t key = readAction();
        if (key == ACTION_MOVE_RIGHT) {
            if (selectedX < 2) {
                selectedX++;
                returnGame();
                drawGame(game, selectedX, selectedY);
            }
        } else if (key == ACTION_MOVE_LEFT) {
            if (selectedX > 0) {
                selectedX--;
                returnGame();
                drawGame(game, selectedX, selectedY);
            }
        } else if (key == ACTION_MOVE_DOWN) {
            if (selectedY < 2) {
                selectedY++;
                returnGame();
                drawGame(game, selectedX, selectedY);
            }
        } else if (key == ACTION_MOVE_UP) {
            if (selectedY > 0) {
                selectedY--;
                returnGame();
                drawGame(game, selectedX, selectedY);
            }
        } else if (key == ACTION_ENTER) {
            if (CELL_AT(game, selectedX, selectedY) == CELL_EMPTY) {
                CELL_AT(game, selectedX, selectedY) = CELL_X;
                returnGame();
                moveCaretUp();
                moveCaretUp();
                clearCurrentLine();
                printf("It is the computer's turn");
                addLine();
                addLine();
                drawGame(game, selectedX, selectedY);

                int winner = checkWinner(game);
                if (winner == CELL_X) {
                    return winner;
                } else if (allCellsFilled(game)) {
                    return CELL_EMPTY;
                }

                sleep(1);
                playAsComputer(game);
                returnGame();
                drawGame(game, selectedX, selectedY);

                winner = checkWinner(game);
                if (winner != CELL_EMPTY) {
                    return winner;
                } else if (allCellsFilled(game)) {
                    return CELL_EMPTY;
                }

                returnGame();
                moveCaretUp();
                moveCaretUp();
                clearCurrentLine();
                printf("It is the player's turn");
                addLine();
                addLine();
                drawGame(game, selectedX, selectedY);
            }
        } else if (key == ACTION_ESCAPE) {
            return CELL_EMPTY;
        }
    }
}

int allCellsFilled(game_t* game_p) {
    for (int i = 0; i < 9; i++) {
        if (game_p->grid[i] == CELL_EMPTY) {
            return 0;
        }
    }
    return 1;
}

void playAsComputer(game_t* game_p) {
    // look for positions with obvious victory conditions
    for (int x = 0; x < 3; x ++) {
        int countO = 0;
        int countEmpty = 0;
        int ey = 0;
        for (int y = 0; y < 3; y++) {
            if (CELL_AT(game_p, x, y) == CELL_O) {
                countO++;
            } else if (CELL_AT(game_p, x, y) == CELL_EMPTY) {
                countEmpty++;
                ey = y;
            }
        }
        if (countO == 2 && countEmpty == 1) {
            CELL_AT(game_p, x, ey) = CELL_O;
            return;
        }
    }
    for (int y = 0; y < 3; y ++) {
        int countO = 0;
        int countEmpty = 0;
        int ex = 0;
        for (int x = 0; x < 3; x++) {
            if (CELL_AT(game_p, x, y) == CELL_O) {
                countO++;
            } else if (CELL_AT(game_p, x, y) == CELL_EMPTY) {
                countEmpty++;
                ex = x;
            }
        }
        if (countO == 2 && countEmpty == 1) {
            CELL_AT(game_p, ex, y) = CELL_O;
            return;
        }
    }
    for (int d = 0; d < 2; d ++) {
        int countO = 0;
        int countEmpty = 0;
        int ex = 0;
        int ey = 0;
        for (int i = 0; i < 3; i++) {
            if (CELL_AT(game_p, d ? 2 - i : i, i) == CELL_O) {
                countO++;
            } else if (CELL_AT(game_p, d ? 2 - i : i, i) == CELL_EMPTY) {
                countEmpty++;
                ex = d ? 2 - i : i;
                ey = i;
            }
        }
        if (countO == 2 && countEmpty == 1) {
            CELL_AT(game_p, ex, ey) = CELL_O;
            return;
        }
    }

    // look for positions with obvious losing conditions
    for (int x = 0; x < 3; x ++) {
        int countX = 0;
        int countEmpty = 0;
        int ey = 0;
        for (int y = 0; y < 3; y++) {
            if (CELL_AT(game_p, x, y) == CELL_X) {
                countX++;
            } else if (CELL_AT(game_p, x, y) == CELL_EMPTY) {
                countEmpty++;
                ey = y;
            }
        }
        if (countX == 2 && countEmpty == 1) {
            CELL_AT(game_p, x, ey) = CELL_O;
            return;
        }
    }
    for (int y = 0; y < 3; y ++) {
        int countX = 0;
        int countEmpty = 0;
        int ex = 0;
        for (int x = 0; x < 3; x++) {
            if (CELL_AT(game_p, x, y) == CELL_X) {
                countX++;
            } else if (CELL_AT(game_p, x, y) == CELL_EMPTY) {
                countEmpty++;
                ex = x;
            }
        }
        if (countX == 2 && countEmpty == 1) {
            CELL_AT(game_p, ex, y) = CELL_O;
            return;
        }
    }
    for (int d = 0; d < 2; d ++) {
        int countX = 0;
        int countEmpty = 0;
        int ex = 0;
        int ey = 0;
        for (int i = 0; i < 3; i++) {
            if (CELL_AT(game_p, d ? 2 - i : i, i) == CELL_X) {
                countX++;
            } else if (CELL_AT(game_p, d ? 2 - i : i, i) == CELL_EMPTY) {
                countEmpty++;
                ex = d ? 2 - i : i;
                ey = i;
            }
        }
        if (countX == 2 && countEmpty == 1) {
            CELL_AT(game_p, ex, ey) = CELL_O;
            return;
        }
    }

    // try middle cell if not taken
    if (CELL_AT(game_p, 1, 1) == CELL_EMPTY) {
        CELL_AT(game_p, 1, 1) = CELL_O;
        return;
    }

    // choose randomly
    int x, y;
    do {
        x = rand() % 3;
        y = rand() % 3;
    } while (CELL_AT(game_p, x, y) != CELL_EMPTY);

    CELL_AT(game_p, x, y) = CELL_O;
}

int checkWinner(game_t* game_p) {
    int firstCell;

    for (int y = 0; y < 3; y++) {
        firstCell = CELL_AT(game_p, 0, y);
        if (firstCell != CELL_EMPTY && firstCell == CELL_AT(game_p, 1, y) && firstCell == CELL_AT(game_p, 2, y)) {
            if (firstCell == CELL_X) {
                return CELL_X;
            } else {
                return CELL_O;
            }
        }
    }

    for (int x = 0; x < 3; x++) {
        firstCell = CELL_AT(game_p, x, 0);
        if (firstCell != CELL_EMPTY && firstCell == CELL_AT(game_p, x, 1) && firstCell == CELL_AT(game_p, x, 2)) {
            if (firstCell == CELL_X) {
                return CELL_X;
            } else {
                return CELL_O;
            }
        }
    }

    firstCell = CELL_AT(game_p, 0, 0);
    if (firstCell != CELL_EMPTY && firstCell == CELL_AT(game_p, 1, 1) && firstCell == CELL_AT(game_p, 2, 2)) {
        if (firstCell == CELL_X) {
            return CELL_X;
        } else {
            return CELL_O;
        }
    }

    firstCell = CELL_AT(game_p, 0, 2);
    if (firstCell != CELL_EMPTY && firstCell == CELL_AT(game_p, 1, 1) && firstCell == CELL_AT(game_p, 2, 0)) {
        if (firstCell == CELL_X) {
            return CELL_X;
        } else {
            return CELL_O;
        }
    }
    
    return CELL_EMPTY;
}
