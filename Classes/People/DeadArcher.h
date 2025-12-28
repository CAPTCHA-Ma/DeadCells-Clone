#ifndef __DEADARCHER_H__
#define __DEADARCHER_H__

#include "Monster.h"
#include <unordered_map>

/**
 * 死亡弓箭手状态枚举
 */
enum class DeadArcherState
{
    idle,   // 待机
    walk,   // 行走/追踪
    dead,   // 死亡
    shoot   // 射击（特有攻击动作）
};

/**
 * 状态动画循环表
 * 定义每个状态播放动画时是否需要循环（例如：行走需循环，射击只需播放一次）
 */
static std::unordered_map<DeadArcherState, bool> DeadArcherStateLoop =
{
    { DeadArcherState::idle,  true  },
    { DeadArcherState::walk,  true  },
    { DeadArcherState::dead,  false },
    { DeadArcherState::shoot, false }
};

/**
 * DeadArcher 类：远程攻击型僵尸怪物
 */
class DeadArcher : public Monster
{
public:
    // Cocos2d-x 标准创建宏，自动实现 create() 函数
    CREATE_FUNC(DeadArcher);

    virtual bool init() override;

    // --- 重写基类行为接口 ---
    virtual void idle() override;
    virtual void walk() override;
    virtual void onDead() override;

    /**
     * 判断当前是否处于攻击动作中
     * 用于防止在攻击动画播放时被 AI 逻辑切换到移动状态
     */
    virtual bool isAttackState() override { return _state == DeadArcherState::shoot; };

    /**
     * AI 决策核心
     * @param dt 每帧时间间隔
     * @param playerWorldPos 玩家当前的世界坐标（用于计算射程和方向）
     */
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) override;

    // --- 状态与动画控制 ---
    void shoot();                                      // 执行射击逻辑（生成箭矢）
    void changeState(DeadArcherState newState);        // 切换有限状态机状态
    void playAnimation(DeadArcherState state, bool loop); // 执行动画切换逻辑

private:
    // 动画缓存：避免每一帧都重复创建 Animation 对象，提高性能
    std::unordered_map<DeadArcherState, cocos2d::Animation*> _animCache;

    float _attackRange;            // 攻击触发范围
    float _moveSpeed;              // 移动速度
    cocos2d::Vec2 _velocity;       // 当前速度向量（处理重力与位移）
    cocos2d::Vec2 _lastPlayerPos;  // 记录玩家最后已知位置，用于锁定射击目标点

    DeadArcherState _state;        // 当前所处的状态
    MoveDirection _direction;      // 怪物朝向（左/右）

    // 内部工具函数
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float time);
    cocos2d::Animation* getAnimation(DeadArcherState state);

    /**
     * 创建攻击判定盒
     * 虽然是远程怪物，但通常也会在身体处创建一个小的判定区处理碰撞
     */
    virtual void createAttackBox() override;

    float _aiTickTimer; // AI 决策计时器，用于控制决策频率（如每0.5秒决策一次）

};

#endif // __DEADARCHER_H__