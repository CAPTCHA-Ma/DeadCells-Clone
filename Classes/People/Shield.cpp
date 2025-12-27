#include "Shield.h"

// 定义数值常量
const float lightningShieldAttack = 50.0f;
const float parryShieldAttack = 50.0f;

Shield* Shield::create(int subTypeIndex)
{
    Shield* s = new Shield((ShieldType)subTypeIndex);
    return s;
}

Shield::Shield(ShieldType type)
    : Weapon(WeaponCategory::Shield), _type(type)
{
    switch (type)
    {
        case ShieldType::LightningShield:
            this->setWeaponAttackPower(lightningShieldAttack);
            break;
        case ShieldType::ParryShield:
            this->setWeaponAttackPower(parryShieldAttack);
            break;
    }
}

Shield::ShieldType Shield::getShieldType()
{
    return _type;
}