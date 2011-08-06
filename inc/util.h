#ifndef UTIL_H
#define UTIL_H

#include <vector>

class Util
{
public:
    static wchar_t bytesToWideChar(const char* bytes, uint32 numBytes);


	template<class t>
	static bool contains(std::vector<t> v, t val) {
		for(typename std::vector<t>::iterator it = v.begin(); it != v.end(); ++it)
			if((*it) == val)
				return true;
		return false;
	}
    /* The return string is null terminated */
    static char* wideCharToBytes(wchar_t c);
};

#endif /* UTIL_H */
