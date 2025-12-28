#include "Shield.h"

// -----------------------------------------------------------------------------
// 数值常量定义：设置不同盾牌的基础攻击力（反伤或反击伤害）
// -----------------------------------------------------------------------------
const float lightningShieldAttack = 50.0f; // 闪电盾攻击力
const float parryShieldAttack = 50.0f;     // 招架盾攻击力

/**
 * 静态工厂创建函数
 * @param subTypeIndex 盾牌子类型的索引（对应枚举 ShieldType）
 */
Shield* Shield::create(int subTypeIndex)
{
    // 创建盾牌实例并返回指针
    Shield* s = new Shield((ShieldType)subTypeIndex);
    // 注意：此处若未调用 autorelease()，外部使用时需手动管理内存或改用 cocos2d 标准 create 流程
    return s;
}

/**
 * 构造函数：根据盾牌类型初始化特定属性
 * @param type 盾牌类型（LightningShield 或 ParryShield）
 */
Shield::Shield(ShieldType type)
    : Weapon(WeaponCategory::Shield), _type(type) // 调用父类 Weapon 构造函数，分类设为 Shield
{
    switch (type)
    {
        case ShieldType::LightningShield:
            // 初始化闪电盾：可能带有电属性反弹伤害
            this->setWeaponAttackPower(lightningShieldAttack);
            break;

        case ShieldType::ParryShield:
            // 初始化招架盾：通常用于触发“完美防御”或反击动作
            this->setWeaponAttackPower(parryShieldAttack);
            break;
    }
}

/**
 * 获取当前盾牌的类型
 * 用于逻辑判定（例如：判断是否能反弹箭矢或阻挡特定爆炸）
 */
Shield::ShieldType Shield::getShieldType()
{
    return _type;
}