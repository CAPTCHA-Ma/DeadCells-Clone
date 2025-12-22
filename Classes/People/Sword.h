// SwordWeapon.h (修正后)
#ifndef __SWORD_H__
#define __SWORD_H__

#include "Weapon.h"
#include <map>
#include <string>

/**
 * @brief SwordWeapon 类：只代表具体的剑类武器。
 */

class Sword : public Weapon
{
public:
    enum class SwordType
    {
        BackStabber,	// 背刺剑
		BaseballBat,        // 棒球棒
        BroadSword,
        OvenAxe
    };
    // 具体的剑类型枚举 
    SwordType getSwordType();
    static Sword* create(int subTypeIndex);
    Sword(SwordType type);
private:
    SwordType _type;
   
};

#endif // __SWORD_WEAPON_H__