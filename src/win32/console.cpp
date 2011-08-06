#include "common.h"

#include "console.h"
#include "console_internal.h"

const uint16 FG_INTENSE = FOREGROUND_INTENSITY;
const uint16 FG_RED     = FOREGROUND_RED;
const uint16 FG_GREEN   = FOREGROUND_GREEN;
const uint16 FG_BLUE    = FOREGROUND_BLUE;
const uint16 BG_INTENSE = BACKGROUND_INTENSITY;

static inline void set(CHAR_INFO* ci, wchar_t c, uint16 colour) {
	ci->Char.UnicodeChar = c;
	ci->Attributes = colour;
}


class ConsoleBuffer : public ConsoleBufferBase<CHAR_INFO>
{
public:
    ConsoleBuffer(uint32 width, uint32 height)
        : ConsoleBufferBase(width, height) {
    }

    virtual void set(uint32 x, uint32 y, wchar_t ch, uint16 colour) {
        assert(x < width && y < height);
		::set(info + (y * width) + x, ch, colour);
    }

    virtual HANDLE getHandle() const
    {
        return NULL;
    }

    friend class ConsoleOutput;
};

class ConsoleOutput : public Console
{
public:
    ConsoleOutput(HANDLE handle)
        : Console(80, 25), handle(handle) {
		CONSOLE_SCREEN_BUFFER_INFO info;
		if (GetConsoleScreenBufferInfo(handle, &info) != 0) {
			width = (info.srWindow.Right - info.srWindow.Left) + 1;
			height = (info.srWindow.Bottom - info.srWindow.Top) + 1;
		}
    }

    void set(uint32 x, uint32 y, wchar_t ch, uint16 colour) {
        CHAR_INFO ci;
        SMALL_RECT rect;
        COORD origin, size;
		::set(&ci, ch, colour);
        rect.Top = y;
        rect.Left = x;
        rect.Bottom = y + 1;
        rect.Bottom = x + 1;
        origin.X = origin.Y = 0;
        size.X = size.Y = 1;
        if (!WriteConsoleOutputW(handle, &ci, size, origin, &rect)) {
            exit(-1);
        }
    }
    void blit(uint32 destX, uint32 destY, uint32 width, uint32 height,
              const Console* source, uint32 srcX = 0, uint32 srcY = 0) {
        const ConsoleBuffer* src = dynamic_cast<const ConsoleBuffer*>(source);
        COORD origin, size;
        SMALL_RECT rect;
        rect.Left = destX;
        rect.Right = destX + width;
        rect.Top = destY;
        rect.Bottom = destY + height;
        origin.X = srcX;
        origin.Y = srcY;
        size.X = src->width;
        size.Y = src->height;
        if (!WriteConsoleOutputW(handle, src->info, size, origin, &rect)) {
            exit(-1);
        }
    }

    virtual HANDLE getHandle() const
    {
        return handle;
    }

	virtual void resize(uint32 w, uint32 h)
	{
		SMALL_RECT rect;
		rect.Left = 0;
		rect.Top = 0;
		rect.Right = w;
		rect.Bottom = h;
		SetConsoleWindowInfo(handle, FALSE, &rect);
	}

private:
    HANDLE handle;
};

Console* Console::createConsoleOutput()
{
    return new ConsoleOutput(CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL));
}

Console* Console::createConsoleBuffer(uint32 width, uint32 height)
{
    return new ConsoleBuffer(width, height);
}
