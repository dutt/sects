#ifndef _WORLD_
#define _WORLD_

#include "item.h"
#include "entity.h"
#include <list>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/upgradable_lock.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

class Square {
public:
	Square(World& w)
		: mSymbol(w, L"Square", L' ') {	}
	wchar_t getSymbol() {
		if(mEntities.empty())
			return ' ';
		EntityIt it = mEntities.end();
		--it;
		return (*it)->getSymbol();
	}
	uint16 getColour() {
		if(mEntities.empty())
			return FG_WHITE;
		EntityIt it = mEntities.end();
		--it;
		return (*it)->getColour();
	}
	Entity* getTopEntity() {
		if(mEntities.empty())
			return 0;
		EntityIt it = --mEntities.end();
		while(!(*it)->isBlocking()) //skip items, they're not blocking
			--it;
		return (*it);
	}
	std::list<Entity*>& getEntities() {
		return mEntities;
	}
	void addEntity(Entity* entity) {
		mEntities.push_back(entity);
	}
	void removeEntity(Entity* entity) {
		mEntities.remove(entity);
	}
	bool isFree() {
		for(EntityIt it = mEntities.begin(); it != mEntities.end(); ++it)
			if((*it)->isBlocking())
				return false;
		return true;
	}
private:
	Object mSymbol; //background
	std::list<Entity*> mEntities;
	std::list<Item*> mItems; //stuff on the ground
	typedef std::list<Entity*>::iterator EntityIt;
	typedef std::list<Item*>::iterator ItemIt;
};

class World {
public:
	typedef boost::interprocess::interprocess_upgradable_mutex MutexType;
	typedef boost::unique_lock<MutexType> WriteLockType;
	typedef boost::interprocess::sharable_lock<MutexType> ReadLockType;

	World(uint32 width, uint32 height);
	wchar_t getSymbol(uint32 x, uint32 y);
	uint16 getColour(uint32 x, uint32 y);
	
	void generate();
	
	void addEntity(Entity* entity, uint32 x, uint32 y);
	void removeEntity(Entity* entity, uint32 x, uint32 y);

	void moveEntity(Entity* entity, uint32 x, uint32 y);
	bool isFree(uint32 x, uint32 y);

	void update();

	ReadLockType getReadLock(uint32 x, uint32 y);
	WriteLockType getWriteLock(uint32 x, uint32 y);
	void releaseLock(ReadLockType& lock);
	void releaseLock(WriteLockType& lock);
private:
	uint32 mWidth;
	uint32 mHeight;
	Square*** mSquares;
	MutexType** mMutexes;
};

#endif //_WORLD
