#include "common.h"

#include "util.h"

wchar_t Util::bytesToWideChar(const char* bytes, uint32 numBytes) {
    wchar_t* retr;
    const char* in;
    size_t reqBytes;
# if HAVE_MBSNRTOWCS
    in = bytes;
    reqBytes = mbsnrtowcs(NULL, &in, numBytes, 0, NULL);
    retr = new wchar_t[reqBytes];
    in = bytes;
    mbsnrtowcs(retr, &in, numBytes, reqBytes, NULL);
# else
    char* tmp = new char[numBytes+1];
    memcpy(tmp, bytes, numBytes);
    tmp[numBytes] = '\0';
    in = tmp;
    reqBytes = mbsrtowcs(NULL, &in, 0, NULL);
    retr = new wchar_t[reqBytes];
    in = tmp;
    mbsrtowcs(retr, &in, reqBytes, NULL);
    delete[] tmp;
# endif
    wchar_t ret = retr[0];
    delete[] retr;
	return ret;
}

char* Util::wideCharToBytes(wchar_t c) {
    const wchar_t* in;
    char out[25];
    size_t len;
    char* ret;
# if HAVE_WCSRTOMBS
    in = &c;
    len = wcsnrtombs(out, &in, 1, sizeof(out), NULL);
# else
    wchar_t tmp[2];
    tmp[0] = c;
    tmp[1] = L'\0';
    in = tmp;
    len = wcsrtombs(out, &in, sizeof(out), NULL);
# endif
    if (len == (size_t)(-1))
        return NULL;
    ret = new char[len + 1];
    memcpy(ret, out, len);
    ret[len] = '\0';
    return ret;
}

