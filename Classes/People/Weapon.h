#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "cocos2d.h"
#include <string>
#include <map>

/**
 * Weapon 类：所有装备（剑、盾、弓）的逻辑基类
 * 注意：该类不继承自 cocos2d::Node，它主要负责数据和逻辑抽象。
 * 具体的渲染由 WeaponNode 或 Player 类处理。
 */
class Weapon
{
public:
    /**
     * @brief 武器的大类枚举
     * 用于在逻辑上区分当前玩家装备的是近战、防御还是远程武器。
     */
    enum class WeaponCategory
    {
        Sword,  // 近战类（如：匕首、阔剑）
        Shield, // 防御类（如：闪电盾、木盾）
        Bow     // 远程类（如：弩、双发弓）
    };

    // 虚析构函数：确保子类（Sword/Shield/Bow）对象被删除时能正确释放内存
    virtual ~Weapon() {}

    /**
     * 静态工厂方法
     * 根据传入的类别枚举，创建并返回具体的子类实例。
     * @param category 想要创建的武器种类
     * @return 返回基类指针指向的子类对象
     */
    static Weapon* create(WeaponCategory category);

    /**
     * 使用 Cocos2d-x 宏自动生成属性的 Getter 和 Setter
     * _category: 存储武器属于哪一大类
     * _weaponAttackPower: 该武器的基础攻击力数值
     */
    CC_SYNTHESIZE(WeaponCategory, _category, Category);
    CC_SYNTHESIZE(float, _weaponAttackPower, WeaponAttackPower);

protected:
    /**
     * 受保护的构造函数
     * 防止外部直接实例化 Weapon 基类，强制通过子类或工厂方法创建。
     */
    Weapon(WeaponCategory category);
};

#endif // __WEAPON_H__