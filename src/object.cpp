#include "object.h"
#include "world.h"

Object::Object(World& w, const std::wstring& name, wchar_t symbol, uint16 colour)
 : mWorld(&w), mX(-1), mY(-1), mName(name), mSymbol(symbol), mColour(colour), mBlocking(false) {

}
Object::Object(World& w, int x, int y, const std::wstring& name, bool isBlocking, wchar_t symbol, uint16 colour)
 : mWorld(&w), mX(x), mY(y), mName(name), mSymbol(symbol), mColour(colour), mBlocking(isBlocking) {

}

void Object::setX(int x) { mX = x; }

void Object::setY(int y) {	mY = y; }

void Object::setPos(int x, int y) {
	mX = x;
	mY = y;
}
