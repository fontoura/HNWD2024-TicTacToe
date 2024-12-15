#ifndef __SETUP_H_DEFINED__
#define __SETUP_H_DEFINED__

// Enum for direction keys
typedef enum {
    ACTION_NONE,
    ACTION_MOVE_UP,
    ACTION_MOVE_DOWN,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_ENTER,
    ACTION_ESCAPE
} Action_t;

void setupTerminal();

void restoreTerminal();

char readCharWithoutEcho();

Action_t readAction();

#endif
