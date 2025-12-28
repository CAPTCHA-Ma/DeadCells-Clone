#ifndef __SHIELD_H__
#define __SHIELD_H__

#include "Weapon.h"

/**
 * Shield 类：继承自 Weapon 武器基类
 * 专门处理具有防御属性的武器逻辑
 */
class Shield : public Weapon
{
public:
    /**
     * @brief 具体的盾牌功能类型枚举
     * LightningShield: 闪电盾，可能带有受击反伤或特效逻辑
     * ParryShield: 招架盾，专注于精准格挡判定
     */
    enum class ShieldType
    {
        LightningShield,
        ParryShield
    };

    /**
     * 静态工厂创建函数
     * @param subTypeIndex 传入类型索引，用于从配置或 UI 中动态生成盾牌
     */
    static Shield* create(int subTypeIndex);

    /**
     * 获取当前盾牌的具体功能类型
     */
    ShieldType getShieldType();

    /**
     * 构造函数
     * @param type 盾牌枚举类型
     */
    Shield(ShieldType type);

private:
    ShieldType _type; // 存储当前盾牌的类型实例
};

#endif