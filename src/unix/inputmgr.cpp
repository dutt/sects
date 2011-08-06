#include "common.h"

#include "inputmgr.h"
#include "windowmgr.h"

#if HAVE_NCURSES_H
# include <ncurses.h>
#else
# include <curses.h>
#endif

static uint8 lastKey = 0;

void InputManager::setup() {
    noecho(); // don't echo characters to the screen
    cbreak(); // don't wait for a complete line before returning characters
    keypad(stdscr, TRUE); // keypad sends KEY_LEFT instead of '4'
    meta(stdscr, TRUE); // want all 8-bit of characters
    nodelay(stdscr, TRUE); // make getch() non-blocking
}

bool InputManager::keyDown() {
    int ret = getch();
    if (ret != ERR) {
        if (ret == KEY_RESIZE) {
            WindowManager::screenResize();
            return keyDown();
        }
        lastKey = (uint8)ret;
        return true;
    }
    return false;
}

void InputManager::updateKeys(bool keys[256]) {
	memset(keys, 0, sizeof(bool) * 256);
    keys[lastKey] = true;
}

const uint8 InputManager::K_UP = (uint8)KEY_UP;
const uint8 InputManager::K_DOWN = (uint8)KEY_DOWN;
const uint8 InputManager::K_LEFT = (uint8)KEY_LEFT;
const uint8 InputManager::K_RIGHT = (uint8)KEY_RIGHT;
const uint8 InputManager::K_ESCAPE = 27;
