#ifndef _ENTITY_
#define _ENTITY_

#include "object.h"
#include "bodypart.h"
#include <list>
#include <map>

class World;
class Player;

namespace EntityAction {
	enum EntityAction_t {
		None = 1,
		Attack = 2,
		CollisionStart = 3,
		CollisionChoice = 4,
		CollisionHandle = 5
	};
}
FLAGSATTRIBUTE(EntityAction::EntityAction_t)

class Entity : public Object {
public:
	Entity(World& w, int x, int y, const std::wstring& name, bool isBlocking, wchar_t symbol, uint16 colour = FG_WHITE);
	virtual ~Entity() {}

	virtual void setWorld(World& world);
	void move(int x, int y);
	virtual void die();

	bool hasAction() { return mHasAction; }
	void resetActions() { mHasAction = true; }
	void actionDone() { mHasAction = false; }

	//called by the engine when the Entity bumps into something
	virtual void handleCollision(Entity* entity) {
		//DataMsg msg;
		//msg.data = A
		entity->dispatchHandleCollision(this);
	}
	//called by the other Entity bumping into this one, shoudl call handleCollision(*this, actions)
	//where actions is all the available actions on this Entity
	virtual void dispatchHandleCollision(Entity* entity) = 0;
	//called by the other Entity this Entity bumps into it with all available actions
	virtual void handleCollision(Entity* entity, EntityAction::EntityAction_t actions) = 0;

	//Message handling
	/*virtual void handleMessage(Message* baseMsg) {
		if(baseMsg->type != MsgType::DataMsgType || baseMsg->dataMsg.length < 4) //invalid message, ignore
			return;
		EntityAction::EntityAction_t type = EntityAction::None;
		_memccpy(&type, baseMsg->dataMsg.data, 0, 4);
		switch(type) {
			case EntityAction::None:
				break;
			case EntityAction::Attack:
				break;
			case EntityAction::CollisionStart: 
				{
					DataMsg newMsg;
					newMsg.data = new uint8[4];
					EntityAction::EntityAction_t data = EntityAction::CollisionChoice;
					_memccpy(newMsg.data, &data, 0, 4);
					newMsg.length = 4;
					sendMessage(newMsg, this, baseMsg->sender);
				}
				break;
			case EntityAction::CollisionChoice:
				throw "Should have been handled in subclass";
				break;
			case EntityAction::CollisionHandle:
				throw "Should have been handled in subclass";
				break;
			default:
				throw "Unknown message";
				break;
		}
	}*/
private:
	bool mHasAction;
};

#endif //_ENTITY_
