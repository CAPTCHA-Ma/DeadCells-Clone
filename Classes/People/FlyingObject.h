#ifndef __FLYINGOBJECT_H__
#define __FLYINGOBJECT_H__

#include "cocos2d.h"
#include "People.h"

/**
 * 飞行物类型枚举
 */
enum class FlyType
{
    Arrow,  // 箭矢（直线弹道）
    Bomb    // 炸弹（抛物线弹道）
};

/**
 * FlyingObject 类：所有飞行道具的基类
 * 继承自 cocos2d::Sprite，使其可以直接作为精灵显示在场景中
 */
class FlyingObject : public cocos2d::Sprite
{
public:
    /**
     * 纯虚函数：启动飞行逻辑
     * 要求所有子类必须实现自己的移动轨迹逻辑（如直线或弧线）
     * @param targetPos 目标位置或方向向量
     */
    virtual void run(cocos2d::Vec2 targetPos) = 0;

    /**
     * 静态工厂方法：统一创建接口
     * 根据类型参数返回对应的子类实例指针
     * @param type 飞行物种类
     * @param fromPlayer 是否为玩家释放（决定伤害判定逻辑）
     * @param atkPower 攻击力数值
     */
    static FlyingObject* create(FlyType type, bool fromPlayer, float atkPower);

protected:
    cocos2d::Vec2 _velocity; // 当前飞行速度向量

    /**
     * 使用宏定义攻击力属性
     * 自动生成 getAttackPower() 和 setAttackPower()
     */
    CC_SYNTHESIZE(float, _attackPower, AttackPower);
};

#endif // __FLYINGOBJECT_H__