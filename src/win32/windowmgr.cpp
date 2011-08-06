#include "common.h"

#include "windowmgr.h"
#include <tchar.h>

typedef struct _CONSOLE_FONT {
	DWORD index;
	COORD dim;
} CONSOLE_FONT;

static bool mSwitch;
static HANDLE mStdHandle;
static Console* mConsoleA, *mConsoleB;
static UINT mOldCodePage;

static COORD mCurrentSize;

void WindowManager::setup() {
	//Set console font
	CONSOLE_FONT_INFOEX lpConsoleCurrentFontEx;
	lpConsoleCurrentFontEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	lpConsoleCurrentFontEx.dwFontSize.X = 12;
	lpConsoleCurrentFontEx.dwFontSize.Y = 12;
	lpConsoleCurrentFontEx.FontWeight = 700;
	lpConsoleCurrentFontEx.nFont = 1;
	lpConsoleCurrentFontEx.FontFamily = FF_DONTCARE;
	lstrcpyW(lpConsoleCurrentFontEx.FaceName, L"Lucida Console");
	SetCurrentConsoleFontEx ( GetStdHandle(STD_OUTPUT_HANDLE), false, &lpConsoleCurrentFontEx );
	
	//get handles and create screen buffers
	mStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	mConsoleA = Console::createConsoleOutput();
	mConsoleB = Console::createConsoleOutput();
	mSwitch = true;

	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(mStdHandle, &info) != 0) {
		mCurrentSize.X = (info.srWindow.Right - info.srWindow.Left) + 1;
		mCurrentSize.Y = (info.srWindow.Bottom - info.srWindow.Top) + 1;
	} else {
		mCurrentSize.X = 80;
		mCurrentSize.Y = 25;
	}

	//get current codepage and set new one, unicode
	mOldCodePage = GetConsoleOutputCP();
	SetConsoleOutputCP(CP_UTF8);
}

BOOL WINAPI SetConsoleFont(HANDLE hOutput, DWORD fontIndex) {
	typedef BOOL (WINAPI *PSetConsoleFont)(HANDLE, DWORD);
	static PSetConsoleFont pSetConsoleFont = NULL;

	if(pSetConsoleFont == NULL)
		pSetConsoleFont = (PSetConsoleFont)::GetProcAddress(::GetModuleHandle(_T("kernel32")), "SetConsoleFont");
	if(pSetConsoleFont == NULL) return FALSE;

	return pSetConsoleFont(hOutput, fontIndex);
}

BOOL WINAPI GetConsoleFontInfo(HANDLE hOutput, BOOL bMaximize, DWORD fontIndex, CONSOLE_FONT* info) {
	typedef BOOL (WINAPI *PGetConsoleFontInfo)(HANDLE, BOOL, DWORD, CONSOLE_FONT*);
	static PGetConsoleFontInfo pGetConsoleFontInfo = NULL;
	if(pGetConsoleFontInfo == NULL)
		pGetConsoleFontInfo = (PGetConsoleFontInfo)::GetProcAddress(::GetModuleHandle(_T("kernel32")), "GetConsoleFontInfo");
	if(pGetConsoleFontInfo == NULL) return FALSE;

	return pGetConsoleFontInfo(hOutput, bMaximize, fontIndex, info);
}

DWORD WINAPI GetNumberOfConsoleFonts() {
	typedef DWORD (WINAPI *PGetNumberOfConsoleFonts)();
	static PGetNumberOfConsoleFonts pGetNumberOfConsoleFonts = NULL;
	if(pGetNumberOfConsoleFonts == NULL)
		pGetNumberOfConsoleFonts = (PGetNumberOfConsoleFonts)::GetProcAddress(::GetModuleHandle(_T("kernel32")), "GetNumberOfConsoleFonts");
	if(pGetNumberOfConsoleFonts == NULL) return 0;
	return pGetNumberOfConsoleFonts();
}

BOOL WINAPI SetConsoleIcon(HICON hIcon) {
	typedef BOOL (WINAPI *PSetConsoleIcon)(HICON);
	static PSetConsoleIcon pSetConsoleIcon = NULL;
	if(pSetConsoleIcon == NULL)
		pSetConsoleIcon = (PSetConsoleIcon)::GetProcAddress(::GetModuleHandle(_T("kernel32")), "SetConsoleIcon");
	if(pSetConsoleIcon == NULL) return FALSE;
	return pSetConsoleIcon(hIcon);
}

void WindowManager::shutdown() {
	SetConsoleActiveScreenBuffer(mStdHandle);
	SetConsoleOutputCP(mOldCodePage);
	delete mConsoleA;
	delete mConsoleB;
}

static void clear(HANDLE hConsole) {
	COORD coordScreen = { 0, 0 };    // here's where we'll home the cursor 
	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi; // to get buffer info 
	DWORD dwConSize;                 // number of character cells in the current buffer 

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);  // get the number of character cells in the current buffer
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);  // fill the entire screen with blanks
	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi); // get the current text attribute
	bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes,	dwConSize, coordScreen, &cCharsWritten); // now set the buffer's attributes accordingly 
	bSuccess = SetConsoleCursorPosition(hConsole, coordScreen); // put the cursor at (0, 0)
	return;
}

void WindowManager::refresh() {
	if(mSwitch) {
		for(WindowIt it = mWindows.begin(); it != mWindows.end(); ++it) {
			(*it)->update(mConsoleA);
		}
		SetConsoleActiveScreenBuffer(mConsoleA->getHandle());
		::clear(mConsoleB->getHandle());
        mSwitch = false;
	}
	else {
		for(WindowIt it = mWindows.begin(); it != mWindows.end(); ++it) {
			(*it)->update(mConsoleB);
		}
		SetConsoleActiveScreenBuffer(mConsoleB->getHandle());
		::clear(mConsoleA->getHandle());
        mSwitch = true;
	}

}

void WindowManager::clear() {
	::clear(mConsoleA->getHandle());
	::clear(mConsoleB->getHandle());
}

uint16 WindowManager::getScreenWidth()
{
	return mCurrentSize.X;
}

uint16 WindowManager::getScreenHeight()
{
	return mCurrentSize.Y;
}
