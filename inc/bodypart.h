#ifndef _BODYPART_
#define _BODYPART_

#include <map>
#include "item.h"

namespace BodyPartAttribute {
	enum BodyPartAttribute_t {
		None = 0,
		Attack, //limb can attack in fights
		Defend,
		Necessary, //creature die without this bodypart, loose concioussness if the part suffers heavy damage
		Dead
	};
}

class Creature;

class BodyPart {
public:
	BodyPart() {}
	BodyPart(Creature* body, const std::wstring& name, uint16 maxHealth,uint16 health = -1);
	BodyPart(Creature* body, const std::wstring& name, uint16 attackRating, uint16 defenceRating, uint16 maxHealth,uint16 health = -1);
	
	inline Creature* getBody() { return mBody; }
	inline uint16 getHealth() { return mHealth; }
	inline uint16 getMaxHealth() { return mMaxHealth; }
	inline void setHealth(uint16 health) { mHealth = health;}
	inline void setMaxHealth(uint16 maxHealth) { mMaxHealth = maxHealth; }
	//0 = still alive, 1 = limb lost, 2 = necessary limb lost, Entity dead
	uint8 changeHealth(int16 delta);

	inline const std::wstring& getName() const { return mName; }
	inline void setName(const std::wstring& name) { mName = name; }

	void clearAttributes();
	void setAttribute(BodyPartAttribute::BodyPartAttribute_t attribute);
	void setAttribute(BodyPartAttribute::BodyPartAttribute_t attribute, double value);
	double getAtttribute(BodyPartAttribute::BodyPartAttribute_t attribute);
	bool hasAttribute(BodyPartAttribute::BodyPartAttribute_t attribute);

private:
	std::map<BodyPartAttribute::BodyPartAttribute_t, double> mAttributes;
	Creature* mBody;
	std::wstring mName;
	uint16 mMaxHealth;
	uint16 mHealth;
};

#endif //_BODYPART_
