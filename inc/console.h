#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstring>

#include "typedefs.h"
#include "common.h"

class Console
{
public:
    virtual ~Console() {
    }

    virtual void set(uint32 x, uint32 y, wchar_t ch, uint16 colour) = 0;
    virtual void blit(uint32 destX, uint32 destY, uint32 width, uint32 height,
                      const Console* source,
                      uint32 srcX = 0, uint32 srcY = 0) = 0;
    void blit(uint32 destX, uint32 destY, const Console* source) {
        blit(destX, destY, source->getWidth(), source->getHeight(), source);
    }

    uint32 getWidth() const {
        return width;
    }
    uint32 getHeight() const {
        return height;
    }

    /* May not be supported by a output console.
     * Will reset the current content to garbage. */
    virtual void resize(uint32 w, uint32 h) = 0;

    static Console* createConsoleOutput();
    static Console* createConsoleBuffer(uint32 width, uint32 height);

#if defined(WIN32) || defined(_WIN32)
    virtual HANDLE getHandle() const = 0;
#endif

protected:
    Console(uint32 width, uint32 height)
        : width(width), height(height) {
    }

    uint32 width, height;
};

extern const uint16 FG_INTENSE;
extern const uint16 FG_RED;
extern const uint16 FG_GREEN;
extern const uint16 FG_BLUE;
extern const uint16 BG_INTENSE;

/* Convenience color values */
const uint16 FG_GREY  = FG_RED | FG_GREEN | FG_BLUE;
const uint16 FG_WHITE = FG_INTENSE | FG_GREY;
const uint16 FG_BLACK = 0;

#if defined(WIN32) || defined(_WIN32)
struct SmallRect {
    SHORT left, top, right, bottom;
};
#else
struct SmallRect {
    int16 left, top, right, bottom;
};
#endif

#endif /* CONSOLE_H */
