#ifndef __BOMB_H__
#define __BOMB_H__

#include "FlyingObject.h"

/**
 * Bomb 类：炸弹飞行物
 * 继承自 FlyingObject，实现了受重力影响的抛物线投掷逻辑及爆炸机制
 */
class Bomb : public FlyingObject
{
public:
    /**
     * 静态工厂创建函数
     * @param attackPower 炸弹爆炸时造成的伤害数值
     */
    static Bomb* create(float attackPower);

    /**
     * 初始化函数
     * 加载炸弹纹理，配置受重力影响的动态刚体，并设置碰撞掩码
     */
    bool init(float attackPower);

    /**
     * 启动投掷逻辑（重写基类方法）
     * 不同于箭矢的直线飞行，炸弹会根据 targetPos 计算复杂的抛物线初速度
     * @param targetPos 投掷的目标点坐标（通常是玩家的世界坐标）
     */
    virtual void run(cocos2d::Vec2 targetPos) override;

    /**
     * 触发爆炸逻辑
     * 停止物理移动，取消碰撞检测，播放缩放/变色的爆炸视觉特效并销毁自身
     */
    void explode();

    /**
     * 查询炸弹是否已经爆炸
     * 用于逻辑判定，防止处于爆炸动画中的炸弹再次触发碰撞
     */
    bool isExploded() const { return _isExploded; }

private:
    bool _isExploded = false; // 爆炸状态标记
    MoveDirection _direction; // 移动方向（虽然炸弹通常是旋转的，但可用作逻辑朝向）
    float _xSpeed = 400.0f;    // 预设参考速度（在抛物线计算中会被动态计算的值覆盖）
};

#endif // __BOMB_H__