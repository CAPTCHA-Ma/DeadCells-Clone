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
	BowType getBowType();
    static Bow* create(int subTypeIndex);
    Bow(BowType type);
private:
    
    BowType _type;
};

#endif
