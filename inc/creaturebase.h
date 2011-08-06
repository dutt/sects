#ifndef _CREATURE_BASE_
#define _CREATURE_BASE_

#include "entity.h"
#include "item.h"
#include <list>

class Player;

namespace CombatAction {
	enum CombatAction_t {
		None,
		Hit,
		Flee
	};
}

class Creature : public Entity {
public:
	Creature(World& w, int x, int y, const std::wstring& name, bool isBlocking, wchar_t symbol, uint16 colour = FG_WHITE);

	void addItem(Item* item) { mItems.push_back(item); }
	std::list<Item*>& getItems() { return mItems; }
	void removeItem(Item* item) { mItems.remove(item); }

	uint32 getNumBodyParts() { return mBodyParts.size(); }
	BodyPart* getBodyPart(uint32 index);
	BodyPart* getBodyPart(std::wstring& name);
	void removeBodyPart(uint32 index);
	void removeBodyPart(std::wstring& name);
	bool hasBodyPart(std::wstring& name);

	//combat
	std::vector<BodyPart*> getDefendingParts();
	std::vector<BodyPart*> getAttackingParts();
	virtual CombatAction::CombatAction_t getCombatAction() { return CombatAction::Hit; }
	inline void setInCombat(bool inCombat) { mInCombat = inCombat; }
	inline bool IsInCombat() { return mInCombat; }

	//called by the other Entity bumping into this one, shoudl call handleCollision(*this, actions)
	//where actions is all the available actions on this Entity
	virtual void dispatchHandleCollision(Entity* entity);
	//called by the other Entity this Entity bumps into it with all available actions
	virtual void handleCollision(Entity* entity, EntityAction::EntityAction_t actions);

	//virtual void handleMessage(Message* baseMsg);
protected:
	std::vector<BodyPart*> getBodyPartsWithAttribute(BodyPartAttribute::BodyPartAttribute_t attribute);
	void addBodyPart(BodyPart* bodypart);
	void addBodyPart(const std::wstring& name, uint16 maxHealth, uint16 health = -1);
	void addBodyPart(const std::wstring& name, uint16 attackRating, uint16 defenceRating, uint16 maxHealth, uint16 health = -1);
	void addNecessaryBodyPart(const std::wstring& name, uint16 maxHealth, uint16 health = -1);
	void addNecessaryBodyPart(const std::wstring& name, uint16 attackRating, uint16 defenceRating, uint16 maxHealth, uint16 health = -1);
private:
	bool mInCombat; //wether the creature is in combat
	uint64 mEffects;
	typedef std::map<std::wstring, double>::iterator AttributeIt;
	std::map<std::wstring, BodyPart*> mBodyParts;
	typedef std::map<std::wstring, BodyPart*>::iterator BodyPartIt;
	std::list<Item*> mItems;
};

class Stationary : public Entity {
public:
	Stationary(World& w, int32 x, int32 y, wchar_t symbol, uint16 colour, const std::wstring& name, bool isBlocking = true) 
		: Entity(w,x,y, name, isBlocking, symbol, colour) {}
	virtual void handleCollision(Entity* entity) { }
	virtual void handleCollision(Entity* entity, EntityAction::EntityAction_t action) {}
	virtual void dispatchHandleCollision(Entity* entity);
};

class RandomWanderer : public Creature {
public:
	RandomWanderer(World& w, int32 x, int32 y, wchar_t symbol, uint16 colour, const std::wstring& name, bool isBlocking = true) 
		: Creature(w,x,y, name,isBlocking, symbol, colour) {}
	void update() {
		int32 dir = rand()%9;
		//0 1 2
		//3   4
		//5 6 7
		switch(dir) {
			case 0: //up left
				move(-1, -1);
				break;
			case 1: //up
				move(0, -1);
				break;
			case 2: //up right
				move(+1, -1);
				break;
			case 3: //left
				move(-1, 0);
				break;
			case 4: //right
				move(+1, 0);
				break;
			case 5: //bottom left
				move(-1, +1);
				break;
			case 6: //bottom
				move(0, +1);
				break;
			case 7: //bottom right
				move(+1, +1);
				break;

		}
	}
};

#endif //_CREATURE_BASE_
