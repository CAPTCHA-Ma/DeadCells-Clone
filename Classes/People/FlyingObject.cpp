#include "FlyingObject.h"
#include "Arrow.h"
#include "Bomb.h"

USING_NS_CC;

/**
 * 飞行物工厂创建函数
 * @param type        飞行物枚举类型 (FlyType::Arrow 或 FlyType::Bomb)
 * @param fromPlayer  是否由玩家发射 (决定了敌我识别判定)
 * @param atkPower    该飞行物携带的攻击力数值
 * @return            指向具体子类实例的基类指针
 */
FlyingObject* FlyingObject::create(FlyType type, bool fromPlayer, float atkPower)
{
    switch (type)
    {
        case FlyType::Arrow:
            // 创建并返回一支箭矢
            return Arrow::create(fromPlayer, atkPower);
            break;

        case FlyType::Bomb:
            // 创建并返回一个炸弹（通常炸弹是怪物的，逻辑相对独立）
            return Bomb::create(atkPower);
            break;

        default:
            // 如果传入了未定义的类型，返回空指针防止崩溃
            return nullptr;
            break;
    }
}