#include "world.h"
//#include "landcreature.h"
//#include "aircreature.h"
//#include "plant.h"
//#include "combatmgr.h"
#include <algorithm>
using namespace std;

World::World(uint32 width, uint32 height) 
: mWidth(width), mHeight(height) {
	mSquares = new Square**[mWidth];
	mMutexes = new MutexType*[mWidth];
	for(uint32 x = 0; x < mWidth; ++x) {
		mSquares[x] = new Square*[mHeight];
		mMutexes[x] = new MutexType[mHeight];
		for(uint32 y = 0; y < mHeight; ++y)
			mSquares[x][y] = new Square(*this);
	}
	generate();
}

wchar_t World::getSymbol(uint32 x, uint32 y) {
	if(x > 0 && x < mWidth && y > 0 && y < mHeight)
		return mSquares[x][y]->getSymbol();
	else
		return L'X';
}
uint16 World::getColour(uint32 x, uint32 y) {
	if(x > 0 && x < mWidth && y > 0 && y < mHeight)
		return mSquares[x][y]->getColour();
	else
		return FG_WHITE;
}

void World::generate() {
	/*//random trees
	for(uint32 x = 0; x < mWidth; ++x)
		for(uint32 y = 0; y < mHeight; ++y){
			if(rand()%10 == 1) {
				//new Tree(*this, x,y);
				Tree::Generate(*this, x, y);
			}
		}
	//crows
	for(uint32 x = 0; x < mWidth; ++x)
		for(uint32 y = 0; y < mHeight; ++y){
			if(rand()%500 == 1) {
				//new Crow(*this, x,y);
				Crow::Generate(*this, x, y);
			}
		}
	//bears
	for(uint32 x = 0; x < mWidth; ++x)
		for(uint32 y = 0; y < mHeight; ++y){
			if(rand()%1000 == 1) {
				//new Bear(*this, x,y);
				Bear::Generate(*this, x, y);
			}
		}*/
}

void World::addEntity( Entity* entity, uint32 x, uint32 y ) {
	mSquares[x][y]->addEntity(entity);
}

void World::removeEntity( Entity* entity, uint32 x, uint32 y ) {
	mSquares[x][y]->removeEntity(entity);
}

void World::moveEntity(Entity *entity, uint32 x, uint32 y) {
	if(x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return;
	if((isFree(x, y) || !entity->isBlocking())) { // && entity->hasAction()) {
		mSquares[entity->getX()][entity->getY()]->removeEntity(entity);
		mSquares[x][y]->addEntity(entity);
		entity->setPos(x,y);
		entity->actionDone();
	} else
		entity->handleCollision(mSquares[x][y]->getTopEntity());
}

bool World::isFree(uint32 x, uint32 y) {
	return mSquares[x][y]->isFree();
}

void World::update() {
	//CombatManager::tick();
	for(uint32 x = 0; x < mWidth; ++x) 
		for(uint32 y = 0; y < mHeight; ++y) 
			for(list<Entity*>::iterator it = mSquares[x][y]->getEntities().begin() ; it != mSquares[x][y]->getEntities().end(); ++it) {
				if((*it)->hasAction()) {
					(*it)->actionDone();
					(*it)->update();
					it = mSquares[x][y]->getEntities().begin();
					if(it == mSquares[x][y]->getEntities().end())
						break;
				}
			}
	for(uint32 x = 0; x < mWidth; ++x) 
		for(uint32 y = 0; y < mHeight; ++y) 
			for(list<Entity*>::iterator it = mSquares[x][y]->getEntities().begin() ; it != mSquares[x][y]->getEntities().end(); ++it) {
				(*it)->resetActions();
			}
}

World::ReadLockType World::getReadLock(uint32 x, uint32 y) {
	return ReadLockType(mMutexes[x][y]);
}

World::WriteLockType World::getWriteLock(uint32 x, uint32 y) {
	return WriteLockType(mMutexes[x][y]);
}

void World::releaseLock(World::ReadLockType& lock) {
	lock.unlock();
}

void World::releaseLock(World::WriteLockType& lock) {
	lock.unlock();
}