// Weapon.h (修正版本)
#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "cocos2d.h"
#include <string>
#include <map>
class Weapon
{
public:
    // 武器的分类
    enum class WeaponCategory
    {
        Sword,
        Shield,
        Bow
    };

    virtual ~Weapon() {}

    // 静态工厂方法
    static Weapon* create(WeaponCategory category);
    CC_SYNTHESIZE(WeaponCategory, _category, Category);
    CC_SYNTHESIZE(int, _attackPower, AttackPower);
    CC_SYNTHESIZE(int, _defensePower, DefensePower);

protected:

    Weapon(WeaponCategory category)
    {
        this->setCategory(category);
        this->setAttackPower(0);
        this->setDefensePower(0);
    }
};

#endif // __WEAPON_H__