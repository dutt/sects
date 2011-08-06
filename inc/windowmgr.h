#ifndef WINDOWMGR_H
#define WINDOWMGR_H

#include "window.h"
#include <list>

class WindowManager {
public:
	static void setup();
	static void shutdown();
	static void addWindow(Window* win) {
		mWindows.push_back(win);
	}
	static void removeWindow(Window* win) {
		mWindows.remove(win);
	}
	static void refresh();
	static void update();
	static void clear();

    static uint16 getScreenWidth();
    static uint16 getScreenHeight();

    static void screenResize();

private:
	static std::list<Window*> mWindows;
	typedef std::list<Window*>::iterator WindowIt;
	WindowManager() {}
};

#endif /* WINDOWMGR_H */
