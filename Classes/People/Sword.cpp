#include "Sword.h"

Sword* Sword::create(int subTypeIndex)
{
    Sword* s = new Sword((SwordType)subTypeIndex);
    return s;
}
Sword::Sword(SwordType type)
    : Weapon(WeaponCategory::Sword), _type(type)
{
    switch (type)
    {
        case SwordType::BackStabber:
			this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
		case SwordType::BaseballBat:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
		case SwordType::BroadSword:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
    }
}
Sword::SwordType Sword::getSwordType()
{
	return _type;
}