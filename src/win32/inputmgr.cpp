#include "common.h"

#include "inputmgr.h"
#include <conio.h>

static HANDLE inHandle;

void InputManager::setup() {
    inHandle = GetStdHandle(STD_INPUT_HANDLE);
}

bool InputManager::keyDown() {
    return _kbhit() != 0;
}

void InputManager::updateKeys(bool keys[256]) {
	for(uint a = 0; a < 256; ++a) {
		keys[a] = (GetKeyState(a) & 0x80) != 0;
    }
    FlushConsoleInputBuffer(inHandle);
}

const uint8 InputManager::K_UP = VK_UP;
const uint8 InputManager::K_DOWN = VK_DOWN;
const uint8 InputManager::K_LEFT = VK_LEFT;
const uint8 InputManager::K_RIGHT = VK_RIGHT;
const uint8 InputManager::K_ESCAPE = VK_ESCAPE;
