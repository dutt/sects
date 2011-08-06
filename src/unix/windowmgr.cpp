#include "common.h"

#include "windowmgr.h"

#if HAVE_NCURSES_H
# include <ncurses.h>
#else
# include <curses.h>
#endif
#include <langinfo.h>

static Console* console = NULL;

static const char* getenc(const char* locale)
{
    if (locale == NULL)
        return NULL;
    const char* enc = strchr(locale, '.');
    if (enc == NULL)
        return NULL;
    return enc + 1;
}

void WindowManager::setup() {
    setlocale(LC_ALL, "");
    char* test = Util::wideCharToBytes((wchar_t)0x12F);
    if (test == NULL) {
        fputs("This terminal need multibyte char support.\n", stderr);
        exit(-1);
    }
    delete[] test;
    const char* x = getenc(getenv("XTERM_LOCALE"));
    if (x != NULL) {
        const char* y = nl_langinfo(CODESET);
        if (strcasecmp(x, y) != 0) {
            fprintf(stderr, "Your terminal (xterm) doesn't use the same codeset (%s) as the current locale is set to (%s).\n", x, y);
            exit(-1);
        }
    }
    initscr();
    nonl();
    console = Console::createConsoleOutput();
}

void WindowManager::shutdown() {
    endwin();
    delete console;
    console = NULL;
}

void WindowManager::refresh() {
    for(WindowIt it = mWindows.begin(); it != mWindows.end(); ++it) {
        (*it)->update(console);
    }
    ::refresh();
}

void WindowManager::screenResize()
{
    console->resize(COLS, LINES);
    for(WindowIt it = mWindows.begin(); it != mWindows.end(); ++it) {
        (*it)->screenResize(COLS, LINES);
        (*it)->update(console);
    }
    ::refresh();
}

uint16 WindowManager::getScreenWidth()
{
    return COLS;
}

uint16 WindowManager::getScreenHeight()
{
    return LINES;
}
