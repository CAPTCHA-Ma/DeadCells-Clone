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
            _defensePower = 10;
            break;

        case ShieldType::IronShield:
            _defensePower = 20;
            break;

        case ShieldType::MagicShield:
            _defensePower = 30;
            break;
    }
}
