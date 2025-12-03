//Weapon.h
#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "cocos2d.h"

class Weapon : public cocos2d::Sprite 
{
public:
    // 武器的分类
    enum class Category 
    {
        Sword,
        Shield,
        Bow
    };
    // 剑类
    enum class SwordType 
    {
        LongSword,
        ShortSword,
        GreatSword
    };
	// 盾类
    enum class ShieldType 
    {
        SmallShield,
        IronShield,
        MagicShield
    };
	// 弓类
    enum class BowType 
    {
        ShortBow,
        LongBow,
        CompositeBow
    };

    
    static Weapon* create(Category weaponCategory, int subTypeIndex);

    bool init(Category weaponCategory, int subTypeIndex);

    CC_SYNTHESIZE(Category, _category, WeaponCategory);
    CC_SYNTHESIZE(int, _attackPower, AttackPower);
    CC_SYNTHESIZE(int, _defensePower, DefensePower);

    
    int _subTypeIndex;

protected:
    Weapon()
        : _category(Category::Sword), _attackPower(0), _defensePower(0), _subTypeIndex(0) 
    {}
    virtual ~Weapon()
    {}
};

#endif // __WEAPON_H__