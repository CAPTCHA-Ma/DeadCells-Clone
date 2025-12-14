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
        case SwordType::AlucardSword:
            _attackPower = 30;
            break;
        case SwordType::BackStabber:
            _attackPower = 15;
            break;
		case SwordType::BaseballBat:
			_attackPower = 20;
            break;
		case SwordType::BroadSword:
            break;
    }
}
Sword::SwordType Sword::getSwordType()
{
	return _type;
}