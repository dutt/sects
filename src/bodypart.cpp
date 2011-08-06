#include "bodypart.h"
#include "creaturebase.h"

BodyPart::BodyPart(Creature* body, const std::wstring& name, uint16 maxHealth,uint16 health /* = -1 */)
: mBody(body), mName(name), mMaxHealth(maxHealth), mHealth(health==(uint16)-1?maxHealth:health) { }

BodyPart::BodyPart(Creature* body, const std::wstring& name, uint16 attackRating, uint16 defenceRating, uint16 maxHealth,uint16 health /* = -1 */)
: mBody(body), mName(name), mMaxHealth(maxHealth), mHealth(health==(uint16)-1?maxHealth:health) {
	if(attackRating > 0)
		setAttribute(BodyPartAttribute::Attack, attackRating);
	if(defenceRating > 0)
		setAttribute(BodyPartAttribute::Defend, defenceRating);
}

void BodyPart::clearAttributes() {
	mAttributes.clear();
}

void BodyPart::setAttribute(BodyPartAttribute::BodyPartAttribute_t attribute) {
	mAttributes[attribute] = 0;
}

void BodyPart::setAttribute(BodyPartAttribute::BodyPartAttribute_t attribute, double value) {
	mAttributes[attribute] = value;
}

double BodyPart::getAtttribute(BodyPartAttribute::BodyPartAttribute_t attribute) {
	return mAttributes[attribute];
}

bool BodyPart::hasAttribute(BodyPartAttribute::BodyPartAttribute_t attribute) {
	return mAttributes.find(attribute) != mAttributes.end();
}

uint8 BodyPart::changeHealth(int16 delta) {
	mHealth = max(0, mHealth + delta);
	if(mHealth <= 0) {
		if(hasAttribute(BodyPartAttribute::Necessary)) {
			getBody()->die();
			return 2;
		}
		else {
			clearAttributes();
			setAttribute(BodyPartAttribute::Dead);
			return 1;
		}
	}
	return 0;
}
