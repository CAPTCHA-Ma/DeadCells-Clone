#include "Bow.h"
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
        case BowType::ShortBow:
            _attackPower = 15;
            break;
        case BowType::LongBow:
            _attackPower = 25;
            break;
        case BowType::CompositeBow:
            _attackPower = 30;
            break;
    }
}