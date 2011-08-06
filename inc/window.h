#ifndef _WINDOW_
#define _WINDOW_

#include <queue>

#include "console.h"

class Window {
public:
	Window(int32 x, int32 y, uint16 width, uint16 height);
	virtual ~Window();

	void update(Console* out);
    virtual void screenResize(uint16 newWidth, uint16 newHeight) = 0;

	void setPos(uint32 x, uint32 y);
	void move(int32 x, int32 y);
	inline void setX(uint x) {
		mX = x;
	}
	inline void setY(uint y) {
		mY = y;
	}
protected:
	void clear(Console* console);
    void resize(uint16 newWidth, uint16 newHeight);
	virtual void update(Console* out, SmallRect& rect) = 0;
	int32 mX, mY;
	uint16 mWidth, mHeight;
    Console* buffer;
};

#include "world.h"

class WorldWindow : public Window {
public:
	WorldWindow(World& world);
	
	void setWorldPos(uint32 x, uint32 y);
	inline void setWorldX(uint32 x) {
		mWorldX = x;
	}
	inline void setWorldY(uint32 y) {
		mWorldY = y;
	}
protected:
	void update(Console* out, SmallRect& rect);
    void screenResize(uint16 newWidth, uint16 newHeight);
private:
	World& mWorld;
	int mWorldX, mWorldY;
};

class LogWindow : public Window {
public:
	static void setup();
	static void log(const std::wstring& text) {
		LogWindow::getInstance()->logInternal(text);
	}
protected:
	void update(Console* out, SmallRect& rect);
    void screenResize(uint16 newWidth, uint16 newHeight);
private:
	void logInternal(const std::wstring& text);
	static LogWindow* getInstance() { return mInstance; }
	static LogWindow* mInstance;
	LogWindow();

	std::deque<std::wstring> mLog;
	typedef std::deque<std::wstring>::iterator LogIt;
};

#endif //_WINDOW_