#ifndef __SHIELD_H__
#define __SHIELD_H__

#include "Weapon.h"

class Shield : public Weapon
{
public:
	enum class ShieldType//具体的盾类型枚举
    {
        SmallShield,
        IronShield,
        MagicShield
    };
    static Shield* create(int subTypeIndex);
    Shield getShieldType();
    Shield(ShieldType type);
private:
    ShieldType  _type;
};

#endif
