#include "common.h"

#if HAVE_NCURSES_H
# include <ncurses.h>
#else
# include <curses.h>
#endif

#include "console.h"
#include "console_internal.h"

const uint16 FG_INTENSE = 0x10;
const uint16 FG_RED     = 0x01;
const uint16 FG_GREEN   = 0x02;
const uint16 FG_BLUE    = 0x04;
const uint16 BG_INTENSE = 0x08;

static inline void set(cchar_t* ch, wchar_t c, uint16 colour) {
    ch->chars[0] = c;
    ch->chars[1] = L'\0';
    ch->attr = (colour & FG_INTENSE) ? A_BOLD : 0;
    ch->attr |= COLOR_PAIR(1 + (colour & 0xf));
}

class ConsoleBuffer : public ConsoleBufferBase<cchar_t>
{
public:
    ConsoleBuffer(uint32 width, uint32 height)
        : ConsoleBufferBase<cchar_t>(width, height) {
    }

    virtual void set(uint32 x, uint32 y, wchar_t ch, uint16 colour) {
        assert(x < width && y < height);
        ::set(info + (y * width) + x, ch, colour);
    }

    friend class ConsoleOutput;
};

class ConsoleOutput : public Console
{
public:
    ConsoleOutput(WINDOW* wnd)
        : Console(COLS, LINES), wnd(wnd) {
    }

    virtual void set(uint32 x, uint32 y, wchar_t ch, uint16 colour)
    {
        cchar_t c;
        ::set(&c, ch, colour);
        wmove(wnd, y, x);
        wecho_wchar(wnd, &c);
    }
    virtual void blit(uint32 destX, uint32 destY, uint32 width, uint32 height,
                      const Console* source, uint32 srcX = 0, uint32 srcY = 0)
    {
        /* TODO: Not very much like a blit this ... probably need to create
         * windows for the ConsoleBuffer instead of this */
        const ConsoleBuffer* src = dynamic_cast<const ConsoleBuffer*>(source);
        const uint32_t endY = destY + height;
        const uint32_t endX = destX + width;
        const cchar_t* srow = src->info + (srcY * src->width);
        if (width == 0)
            return;
        for (uint32 y = destY; y < endY; ++y) {
            uint32 x = destX;
            const cchar_t* sc = srow + srcX;
            mvwadd_wch(wnd, y, x++, sc++);
            for (; x < endX; ++x, ++sc)
                wadd_wch(wnd, sc);
            srow += src->width;
        }
    }

    virtual void resize(uint32 newWidth, uint32 newHeight)
    {
        width = newWidth;
        height = newHeight;
    }

private:
    WINDOW* wnd;
};


Console* Console::createConsoleOutput() {
    if (!has_colors()) {
        fputs("This terminal need colors.\n", stderr);
        exit(-1);
    }
    curs_set(0);
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);
    init_pair(8, COLOR_WHITE, COLOR_BLACK);
    init_pair(9, COLOR_BLACK, COLOR_WHITE);
    init_pair(10, COLOR_RED, COLOR_WHITE);
    init_pair(11, COLOR_GREEN, COLOR_WHITE);
    init_pair(12, COLOR_YELLOW, COLOR_WHITE);
    init_pair(13, COLOR_BLUE, COLOR_WHITE);
    init_pair(14, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(15, COLOR_CYAN, COLOR_WHITE);
    init_pair(16, COLOR_WHITE, COLOR_WHITE);
    return new ConsoleOutput(stdscr);
}

Console* Console::createConsoleBuffer(uint32 width, uint32 height)
{
    return new ConsoleBuffer(width, height);
}
