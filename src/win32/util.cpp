#include "common.h"

#include "util.h"

wchar_t Util::bytesToWideChar(const char* bytes, uint32 numBytes) {
    wchar_t* retr;
	int reqBytes = MultiByteToWideChar(CP_UTF8, 0, bytes, numBytes, 0, 0);
    retr = new wchar_t[reqBytes];
	MultiByteToWideChar(CP_UTF8, 0, bytes, numBytes, retr, reqBytes);
    wchar_t ret = retr[0];
    delete[] retr;
	return ret;
}

char* Util::wideCharToBytes(wchar_t c) {
	int reqBytes = WideCharToMultiByte(CP_UTF8, 0, &c, 1, NULL, 0, NULL, NULL);
	char* bytes = new char[reqBytes+1];
	int got = WideCharToMultiByte(CP_UTF8, 0, &c, 1, bytes, reqBytes, NULL, NULL);
	bytes[got] = '\0';
	return bytes;
}
