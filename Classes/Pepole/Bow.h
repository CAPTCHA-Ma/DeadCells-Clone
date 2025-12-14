#ifndef __BOW_H__
#define __BOW_H__

#include "Weapon.h"

class Bow : public Weapon
{
public:
    enum class BowType
    {
        ShortBow,
        LongBow,
        CompositeBow
    };

    static Bow* create(int subTypeIndex);
private:
    Bow(BowType type);
    CC_SYNTHESIZE(BowType, _type, Type);
};

#endif
