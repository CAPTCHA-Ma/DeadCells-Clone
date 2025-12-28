#include "Sword.h"

// -----------------------------------------------------------------------------
// 数值常量定义：各类近战武器的基础攻击力
// -----------------------------------------------------------------------------
const float backStabberAttackPower = 30.0f; // 匕首/背刺者
const float baseballBatAttackPower = 40.0f; // 棒球棍
const float broadSwordAttackPower = 50.0f; // 阔剑
const float OvenAxeAttackPower = 50.0f; // 斧头

/**
 * 静态工厂创建函数
 * @param subTypeIndex 武器子类型的索引（对应 SwordType 枚举）
 */
Sword* Sword::create(int subTypeIndex)
{
    // 创建实例并强转枚举类型
    Sword* s = new Sword((SwordType)subTypeIndex);
    return s;
}

/**
 * 构造函数：初始化武器类别并根据具体类型设置攻击力
 */
Sword::Sword(SwordType type)
    : Weapon(WeaponCategory::Sword), _type(type)
{
    switch (type)
    {
        case SwordType::BackStabber:
            this->setWeaponAttackPower(backStabberAttackPower);
            break;

        case SwordType::BaseballBat:
            this->setWeaponAttackPower(baseballBatAttackPower);
            break;

        case SwordType::BroadSword:
            this->setWeaponAttackPower(broadSwordAttackPower);
            break;

        case SwordType::OvenAxe:
            this->setWeaponAttackPower(OvenAxeAttackPower);
            break;
    }
}

/**
 * 获取当前近战武器的类型
 * 可用于判断攻击范围、播放不同的挥砍音效或特效
 */
Sword::SwordType Sword::getSwordType()
{
    return _type;
}