#ifndef __GRENADIER_H__
#define __GRENADIER_H__

#include "Monster.h"
#include <unordered_map>

/**
 * 掷弹兵状态枚举
 */
enum class GrenadierState
{
    idle,   // 待机
    atk,    // 攻击（投掷炸弹）
    walk,   // 行走/追踪
    dead    // 死亡
};

/**
 * 状态动画循环配置表
 * true 表示动画循环播放（如待机、走路），false 表示只播放一次（如攻击、死亡）
 */
static std::unordered_map<GrenadierState, bool> GrenadierStateLoop =
{
    { GrenadierState::idle, true  },
    { GrenadierState::atk,  false },
    { GrenadierState::walk,  true } ,
    { GrenadierState::dead,  false }
};

/**
 * Grenadier 类：投掷型远程怪物
 * 继承自 Monster 基类
 */
class Grenadier : public Monster
{
public:
    // Cocos2d-x 标准创建宏
    CREATE_FUNC(Grenadier);

    virtual bool init() override;

    // --- 核心行为重写 ---
    virtual void idle() override;
    virtual void walk() override;
    virtual void onDead() override;

    /**
     * 投掷炸弹攻击逻辑
     */
    void attack();

    /**
     * AI 决策函数
     * @param dt 帧间隔时间
     * @param playerWorldPos 玩家的世界坐标，用于计算投掷落点
     */
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) override;

    /**
     * 判断当前是否处于攻击动作中
     */
    virtual bool isAttackState() override { return _state == GrenadierState::atk; };

    // --- 状态与动画管理 ---
    void changeState(GrenadierState newState);
    void playAnimation(GrenadierState state, bool loop);

private:
    // 动画资源缓存，提升性能，避免重复创建动画对象
    std::unordered_map<GrenadierState, cocos2d::Animation*> _animCache;

    float _attackRange;            // 攻击范围（炸弹能投掷的最远距离）
    float _moveSpeed;              // 移动速度
    cocos2d::Vec2 _velocity;       // 物理速度向量
    cocos2d::Vec2 _lastPlayerPos;  // 记录玩家最后的位置，用于炸弹的抛物线落点计算

    GrenadierState _state;         // 当前状态机状态
    MoveDirection _direction;      // 怪物朝向

    // 内部工具函数：创建和获取动画
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float time);
    cocos2d::Animation* getAnimation(GrenadierState state);

    /**
     * 创建攻击判定盒
     * 对于掷弹兵，除了炸弹本身，身体周围也可能有一个物理检测区
     */
    virtual void createAttackBox() override;

    float _aiTickTimer; // AI 决策计时器，控制逻辑更新频率

};

#endif // __GRENADIER_H__