#ifndef _ITEM_
#define _ITEM_

#include "object.h"

class Item : Object {
public:
	Item(World& w, int x, int y, const std::wstring& name, wchar_t symbol, uint16 colour = FG_INTENSE, uint32 worth = 0) 
		: Object(w, x, y, name, false, symbol, colour), mName(name), mWorth(worth) { }
	Item(World& w, const std::wstring& name, wchar_t symbol, uint16 colour = FG_INTENSE, uint32 worth = 0)
		: Object(w, name, false, colour), mName(name), mWorth(worth) { }
	std::wstring& getName() { return mName; }
	inline uint32 getWorth() { return mWorth; }
	void setWorth(uint32 worth) { mWorth = worth; }
	Item* Generate(World& w);
protected:
	static void reportError(const char* funcName, int val);
private:
	std::wstring mName;
	uint32 mWorth;
};

class Weapon : public Item {
public:
	Weapon(World& w, int x, int y, const std::wstring& name, wchar_t symbol, uint16 colour, uint32 worth, uint32 minDamage, uint32 maxDamage = -1) 
		: Item(w, x, y, name, symbol, colour, worth), mMinDamage(minDamage), mMaxDamage((maxDamage == ((uint32)-1)?minDamage:maxDamage)) { }
	Weapon(World& w, const std::wstring& name, wchar_t symbol, uint16 colour, uint32 worth, uint32 minDamage, uint32 maxDamage = -1)
		: Item(w, name, symbol, colour, worth), mMinDamage(minDamage), mMaxDamage((maxDamage == ((uint32)-1)?minDamage:maxDamage)) { }

	static Weapon* Generate(World& w);
private:
	uint32 mMinDamage, mMaxDamage;
};

#endif /* _ITEM_ */
