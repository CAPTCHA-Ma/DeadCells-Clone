#ifndef __BOW_H__
#define __BOW_H__

#include "Weapon.h"

/**
 * Bow 类：继承自 Weapon 基类
 * 实现了游戏中所有弓弩类武器的分类与特有属性
 */
class Bow : public Weapon
{
public:
    /**
     * @brief 弓的具体子类型枚举
     * closeCombatBow: 近战弓（可能带有近距离伤害加成）
     * dualBow: 双发弓（可能一次发射两枚箭矢）
     * crossbow: 弩（通常射速较快或带有穿透属性）
     */
    enum class BowType
    {
        closeCombatBow,
        dualBow,
        crossbow
    };

    /**
     * 静态工厂创建函数
     * @param subTypeIndex 传入类型索引，对应上面的 BowType 枚举
     */
    static Bow* create(int subTypeIndex);

    /**
     * 获取当前弓的类型
     */
    BowType getBowType();

    /**
     * 构造函数
     * @param type 弓的具体类型
     */
    Bow(BowType type);

    /**
     * 宏定义属性：爆炸攻击力
     * 使用 CC_SYNTHESIZE 自动生成 getWeaponAttackPower() 和 setWeaponAttackPower() 方法
     * 用于处理某些特殊弓箭触发的爆炸性伤害数值
     */
    CC_SYNTHESIZE(float, _boomAttackPower, WeaponAttackPower);

private:
    BowType _type; // 存储当前弓的种类
};

#endif