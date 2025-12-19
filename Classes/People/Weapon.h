#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "cocos2d.h"
#include <string>
#include <map>
#include "People.h"

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
	CC_SYNTHESIZE(BasicAttributes, _weaponAttributes, WeaponAttributes);

protected:
    Weapon(WeaponCategory category);
};

#endif // __WEAPON_H__