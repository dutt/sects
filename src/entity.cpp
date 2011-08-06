#include "entity.h"
#include "world.h"

using namespace std;

Entity::Entity(World& w, int x, int y, const std::wstring& name, bool isBlocking, wchar_t symbol, uint16 colour) 
: Object(w, x, y, name, isBlocking, symbol, colour) {
	setWorld(w);
}

void Entity::setWorld(World& world) {
	World& w = getWorld();
	World::WriteLockType lock = w.getWriteLock(getX(), getY());
	if(hasWorld()) {
		w.removeEntity(this, getX(), getY());
	}
	w.addEntity(this, getX(), getY());
	w.releaseLock(lock);
	Object::setWorld(world);
}

void Entity::move(int x, int y) { 
	World& w = getWorld();
	World::WriteLockType lock = w.getWriteLock(getX(), getY());
	w.moveEntity(this, getX() + x, getY() + y); 
	w.releaseLock(lock);
}

void Entity::die() {
	if(hasWorld()) {
		World& w = getWorld();
		World::WriteLockType lock = w.getWriteLock(getX(), getY());
		w.removeEntity(this, getX(), getY());
		w.releaseLock(lock);
	}
}
