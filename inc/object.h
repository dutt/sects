#ifndef _OBJECT_
#define _OBJECT_

#include "common.h"
#include "util.h"
#include "console.h"

class World;

//base class for everything
class Object {
public:
	Object(World& w, const std::wstring& name, wchar_t symbol, uint16 colour = FG_WHITE);
	//Object(World& w, const std::wstring& name, uint16 colour = FG_WHITE);
	//Object(World& w, int x, int y, const std::wstring& name, bool isBlocking, uint16 colour = FG_WHITE);
	Object(World& w, int x, int y, const std::wstring& name, bool isBlocking, wchar_t symbol, uint16 colour = FG_WHITE);

	void setPos(int x, int y);
	void setX(int x);
	int getX() { return mX;}
	void setY(int y);
	int getY() { return mY; }

	inline void setSymbol(wchar_t symbol) {	mSymbol = symbol;	}
	void setSymbol(const char* bytes, uint32 numBytes) {
        mSymbol = Util::bytesToWideChar(bytes, numBytes);
    }
	inline wchar_t getSymbol() { return mSymbol; }
	inline uint16 getColour() { return mColour; }
	inline void setColour(uint16 colour) { mColour = colour; }
	inline void setSymbolColour(wchar_t symbol, uint16 colour) {
		setSymbol(symbol);
		setColour(colour);
	}

	inline bool isVisible() { return mVisible; }
	inline void setVisible(bool visible) { mVisible = visible; }

	inline bool hasWorld() { return mWorld != 0; }
	virtual void setWorld(World& world) { mWorld = &world; }
	inline World& getWorld() { return *mWorld; }

	virtual void update() {};

	std::wstring& getName() { return mName; }

	inline bool isBlocking() { return mBlocking; }
private:
	World* mWorld;
	int mX, mY;
	std::wstring mName;
	wchar_t mSymbol;
	uint16 mColour;
	bool mVisible;
	bool mBlocking;
};

#endif //_OBJECT_
