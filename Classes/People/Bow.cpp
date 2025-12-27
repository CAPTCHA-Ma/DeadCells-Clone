#include "Bow.h"

// 定义数值常量，方便统一调整
const float closeCombatBowAttack = 15.0f;
const float dualBowAttack = 20.0f;
const float crossbowAttack = 45.0f;

Bow* Bow::create(int subTypeIndex)
{
    Bow* b = new Bow((BowType)subTypeIndex);
    return b;
}

Bow::Bow(BowType type)
    : Weapon(WeaponCategory::Bow), _type(type)
{
    switch (type)
    {
        case BowType::closeCombatBow:
            this->setWeaponAttackPower(closeCombatBowAttack);
            break;
        case BowType::dualBow:
            this->setWeaponAttackPower(dualBowAttack);
            break;
        case BowType::crossbow:
            this->setWeaponAttackPower(crossbowAttack);
            break;
    }
}
Bow::BowType Bow::getBowType()
{
    return _type;
}