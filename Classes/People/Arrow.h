#ifndef __ARROW_H__
#define __ARROW_H__

#include "FlyingObject.h"

/**
 * Arrow 类：箭矢飞行物
 * 继承自 FlyingObject，实现了直线飞行的物理投射物逻辑
 */
class Arrow : public FlyingObject
{
public:
    /**
     * 静态工厂创建函数
     * @param fromPlayer 是否为玩家射出（决定了碰撞掩码，防止伤到队友）
     * @param attackPower 箭矢携带的攻击力
     */
    static Arrow* create(bool fromPlayer, float attackPower);

    /**
     * 初始化函数
     * 设置箭矢的初始物理外观、刚体属性和碰撞掩码
     */
    bool init(bool fromPlayer, float attackPower);

    /**
     * 启动箭矢飞行逻辑（重写基类方法）
     * @param targetPos 飞行的方向向量（通常是单位向量，如 Vec2(1, 0)）
     */
    virtual void run(cocos2d::Vec2 targetPos) override;

    /**
     * 命中处理函数
     * 当物理引擎检测到箭矢触碰目标或墙壁时调用，负责停止飞行并播放销毁效果
     */
    void hit();

    /**
     * 查询箭矢是否已经命中
     * 防止在同一帧内多次触发伤害逻辑
     */
    bool hasHit() { return _hasHit; };

private:
    MoveDirection _direction; // 箭矢的朝向（左/右），用于控制 Sprite 的翻转
    float _speed = 400.0f;    // 箭矢的飞行初速度
    bool _hasHit;             // 命中状态标记位
};

#endif // __ARROW_H__