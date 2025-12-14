#ifndef __SHIELD_H__
#define __SHIELD_H__

#include "Weapon.h"

class Shield : public Weapon
{
public:
    enum class ShieldType
    {
        SmallShield,
        IronShield,
        MagicShield
    };
    static Shield* create(int subTypeIndex);
private:
    Shield(ShieldType type);
    CC_SYNTHESIZE(ShieldType, _type, Type);
};

#endif
