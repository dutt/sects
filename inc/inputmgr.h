#ifndef INPUTMGR_H
#define INPUTMGR_H

class InputManager
{
public:
    static void setup();
    static bool keyDown();
    static void updateKeys(bool keys[256]);

    static const uint8 K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE;
};

#endif /* INPUTMGR_H */
