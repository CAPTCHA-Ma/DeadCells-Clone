#include "Sword.h"
const float backStabberAttackPower = 30.0f;
const float baseballBatAttackPower = 40.0f;
const float broadSwordAttackPower =  50.0f;
const float OvenAxeAttackPower    =    50.0f;
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
            this->setWeaponAttackPower(backStabberAttackPower);
            break;
		case SwordType::BaseballBat:
            this->setWeaponAttackPower(baseballBatAttackPower);
            break;
		case SwordType::BroadSword:
            this->setWeaponAttackPower(broadSwordAttackPower);
        case SwordType::OvenAxe:
            this->setWeaponAttackPower(OvenAxeAttackPower);
            break;
    }
}
Sword::SwordType Sword::getSwordType()
{
	return _type;
}