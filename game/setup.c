#include "setup.h"
#include <stdio.h>

#ifdef _WIN32
    #include <conio.h>  // For Windows
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <termios.h>
    #include <unistd.h>  // For Unix-like systems
    #include <sys/select.h> // for select()
    #include <fcntl.h>
#endif

// Function to set up the terminal (does nothing for Windows)
void setupTerminal() {
#ifndef _WIN32
    struct termios newt;
    
    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &newt);

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);

    // Apply new settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#else
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut == INVALID_HANDLE_VALUE) {
        return;
    }
    if (!GetConsoleMode(hOut, &dwMode)) {
        return;
    }
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

void restoreTerminal() {
    #ifndef _WIN32
    struct termios newt;
    
    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &newt);

    // Disable canonical mode and echo
    newt.c_lflag |= (ICANON | ECHO);

    // Apply new settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
}

// Function to read a character without echoing
char readCharWithoutEcho() {
#ifdef _WIN32
    // Use getch() on Windows
    return getch();
#else
    // Use getchar() on Unix-like systems
    return getchar();
#endif
}

Action_t readAction() {
    char ch;

#ifdef _WIN32
    /*ch = readCharWithoutEcho();

    // Check for ENTER and ESC keys
    if (ch == 13) return ACTION_ENTER; // ENTER key in Windows
    if (ch == 27) return ACTION_ESCAPE; // ESC key

    // On Windows, arrow keys are preceded by 0 or 224
    if (ch == 0 || ch == 224) {
        ch = readCharWithoutEcho(); // The actual arrow key code
        switch (ch) {
            case 72: return ACTION_MOVE_UP;
            case 80: return ACTION_MOVE_DOWN;
            case 75: return ACTION_MOVE_LEFT;
            case 77: return ACTION_MOVE_RIGHT;
            default: return ACTION_NONE;
        }
    } else {
        return ACTION_NONE;
    }*/
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;
    DWORD events;
    Action_t action = ACTION_NONE;

    // Read console input
    while (1) {
        ReadConsoleInput(hStdin, &inputRecord, 1, &events);

        // Check if it's a key event
        if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
            KEY_EVENT_RECORD key = inputRecord.Event.KeyEvent;

            switch (key.wVirtualKeyCode) {
                case VK_RETURN:  // Enter key
                    return ACTION_ENTER;
                case VK_ESCAPE:  // Escape key
                    return ACTION_ESCAPE;
                case VK_UP:      // Arrow up
                    return ACTION_MOVE_UP;
                case VK_DOWN:    // Arrow down
                    return ACTION_MOVE_DOWN;
                case VK_LEFT:    // Arrow left
                    return ACTION_MOVE_LEFT;
                case VK_RIGHT:   // Arrow right
                    return ACTION_MOVE_RIGHT;
                default:
                    break;
            }
        }
    }

    return action;

#else
    // On Unix-like systems, arrow keys start with ESC (27) and are followed by '['
    ch = readCharWithoutEcho();
    if (ch == 10) return ACTION_ENTER; // ENTER key on Unix

    if (ch == 27) { // ESC key
        usleep(100000); // Sleep for 100ms to wait for potential additional characters

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

        ch = readCharWithoutEcho();

        flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);

        if (ch == -1) {
            return ACTION_ESCAPE;
        }

        if (ch == '[') {
            ch = readCharWithoutEcho(); // The actual arrow key code
            switch (ch) {
                case 'A': return ACTION_MOVE_UP;
                case 'B': return ACTION_MOVE_DOWN;
                case 'D': return ACTION_MOVE_LEFT;
                case 'C': return ACTION_MOVE_RIGHT;
                default: return ACTION_NONE;
            }
        }
    }
    return ACTION_NONE;
#endif
}
