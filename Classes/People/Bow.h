#ifndef __BOW_H__
#define __BOW_H__

#include "Weapon.h"

class Bow : public Weapon
{
public:
    enum class BowType
    {
        closeCombatBow,
        dualBow,
        crossbow
    };
    static Bow* create(int subTypeIndex);
	BowType getBowType();
    Bow(BowType type);
    CC_SYNTHESIZE(float, _boomAttackPower, WeaponAttackPower);
private:
    
    BowType _type;
};

#endif
