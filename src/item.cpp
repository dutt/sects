#include "item.h"
#include <sstream>
using namespace std;

Item* Item::Generate(World& w) {
	int val = rand()%2;
	switch(val) {
		case 0:
			return Weapon::Generate(w);
		case 1:
			return new Item(w, L"Coin", L'x', FG_INTENSE, 1);
		default: {
			reportError("Item::Generate", val);
		}
	}
	throw "should not be here";
}

void Item::reportError(const char* funcName, int val) {
	stringstream ss;
	ss <<funcName <<", val had an unknown value: " <<val;
	throw ss.str();
}

Weapon* Weapon::Generate(World& w) {
	int val = rand()%4;
	switch(val) {
			case 0:
				return new Weapon(w, L"Sword", (wchar_t)L'/', FG_INTENSE, 10, 4, 5);
				break;
			case 1:
				return new Weapon(w, L"Spear", (wchar_t)L'|', FG_INTENSE, 10, 4, 5);
				break;
			case 2:
				return new Weapon(w, L"Axe", (wchar_t)L'x', FG_INTENSE, 10, 4, 5);
				break;
			default: {
				reportError("Weapon::Generate", val);
			}
	}
	throw "should not be here";
}
