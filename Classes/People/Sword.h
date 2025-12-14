// SwordWeapon.h (修正后)
#ifndef __SWORD_WEAPON_H__
#define __SWORD_WEAPON_H__

#include "Weapon.h"
#include <map>
#include <string>

/**
 * @brief SwordWeapon 类：只代表具体的剑类武器。
 */
class Sword : public Weapon
{
public:
    // 具体的剑类型枚举 
    enum class SwordType
    {
        AlucardSword,  // 阿鲁卡多剑
        ShortSword,    // 短剑
        GreatSword     // 双手大剑

    };
    static Sword* create(int subTypeIndex);
private:
    Sword(SwordType type);
    CC_SYNTHESIZE(SwordType, _type, Type);
};

#endif // __SWORD_WEAPON_H__