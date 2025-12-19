#include "Shield.h"

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
        case ShieldType::SmallShield:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
        case ShieldType::IronShield:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
        case ShieldType::MagicShield:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
    }
}
Shield Shield::getShieldType()
{
    return _type;
}
