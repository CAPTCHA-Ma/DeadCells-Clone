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
        case ShieldType::LightningShield:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
        case ShieldType::ParryShield:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
    }
}
Shield::ShieldType Shield::getShieldType()
{
    return _type;
}
