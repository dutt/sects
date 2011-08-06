#include "common.h"
#include "window.h"
#include "windowmgr.h"

Window::Window(int32 x, int32 y, uint16 width, uint16 height) 
    : mX(x), mY(y), mWidth(width), mHeight(height),
      buffer(Console::createConsoleBuffer(width, height)) {
	WindowManager::addWindow(this);
}

Window::~Window() {
	WindowManager::removeWindow(this);
    delete buffer;
}

static inline uint16 clamp(int32 v, uint16 _max)
{
    return min(max(v, 0), (int32)_max);
}

static inline int32 getValidX(int32 x)
{
    return clamp(x, WindowManager::getScreenWidth());
}

static inline int32 getValidY(int32 y)
{
    return clamp(y, WindowManager::getScreenHeight());
}

void Window::update(Console* out) {
	SmallRect rect;
	rect.top = getValidY(mY);
	rect.left = getValidX(mX);
	rect.bottom = getValidY(mY+mHeight);
	rect.right = getValidX(mX+mWidth);
	update(out, rect);
}

void Window::resize(uint16 newWidth, uint16 newHeight) {
    mWidth = newWidth;
    mHeight = newHeight;
    buffer->resize(newWidth, newHeight);
}

void Window::move(int32 x, int32 y) {
	setX(mX + x);
	setY(mY + y);
}

//
// WorldWindow
///////////////////////////////////////////////////////////////////////////////////

WorldWindow::WorldWindow(World& world)
: Window(0, 0, WindowManager::getScreenWidth(),
         WindowManager::getScreenHeight()), mWorld(world) {
	mWorldX = mWorldY = 0;
}

void WorldWindow::update(Console* out, SmallRect& rect) {
    for (uint32 x = 0; x < mWidth; ++x) {
        for (uint32 y = 0; y < mHeight; ++y) {
            buffer->set(x, y, mWorld.getSymbol(mWorldX + x, mWorldY + y),
                        mWorld.getColour(mWorldX + x, mWorldY + y));
        }
	}
    out->blit(rect.left, rect.top,
              rect.right - rect.left, rect.bottom - rect.top, buffer);
}

void WorldWindow::screenResize(uint16 newWidth, uint16 newHeight) {
    resize(newWidth - mX, newHeight - mY);
}

void Window::setPos(uint32 x, uint32 y) {
	setX(x);
	setY(y);
}

void WorldWindow::setWorldPos(uint32 x, uint32 y) {
	setWorldX(x);
	setWorldY(y);
}

//
// LogWindow
///////////////////////////////////////////////////////////////////////////////////

LogWindow* LogWindow::mInstance = 0;

void LogWindow::setup() {
	mInstance = new LogWindow();
}

LogWindow::LogWindow()
: Window(0, WindowManager::getScreenHeight() - 6, 60, 6) {
}

void LogWindow::update(Console* out, SmallRect& rect) {
	for(uint32 y = 0; y < mHeight; ++y) {
		for(uint32 x = 0; x < mWidth; ++x) {
			if(x == 0 || y == 0 || x == mWidth-1 || y == mHeight-1) { //border
				buffer->set(x, y, L' ', BG_INTENSE);
			}
			else if(y <= mLog.size() && x-1 < mLog[y-1].length()) {
                buffer->set(x, y, mLog[y-1][x-1], FG_INTENSE);
			}
			else
			{
				buffer->set(x, y, L' ', FG_INTENSE);
			}
		}
	}
    out->blit(rect.left, rect.top,
              rect.right - rect.left, rect.bottom - rect.top, buffer);
}

void LogWindow::screenResize(uint16 newWidth, uint16 newHeight) {
    setY(WindowManager::getScreenHeight() - 6);
}

void LogWindow::logInternal(const std::wstring& text) {
	while((uint16)mLog.size() >= mHeight-1)
		mLog.pop_front();
	mLog.push_back(text);
}
