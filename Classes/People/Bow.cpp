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
        case BowType::closeCombatBow:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
        case BowType::dualBow:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
        case BowType::crossbow:
            this->setWeaponAttributes(BasicAttributes({ 0, 30, 0 }));
            break;
    }
}
Bow::BowType Bow::getBowType()
{
    return _type;
}