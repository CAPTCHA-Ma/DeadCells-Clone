// Player.h
#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "cocos2d.h"
#include <unordered_map>
#include "Weapon.h"  // 确保 Weapon.h 存在
struct StateConfig
{
    bool canBeInterrupted;
    int priority;
    bool loop;
};
enum class ActionState 
{
    stand,      // 站立
    run,        // 奔跑
    jumpUp,
    crouch,     // 下蹲
    atkA,   // 徒手攻击 (或通用攻击)
    atkB,
    AtkAlucardSwordA,
    rollStart
};
enum class MoveDirection 
{
    NONE,
    LEFT,
    RIGHT
};
static std::unordered_map<ActionState, StateConfig> StateTable =
{
    { ActionState::stand,     { true,  0, true  } },
    { ActionState::run,       { true,  1, true  } },
    { ActionState::jumpUp,    { false, 2, false } },
    { ActionState::atkA,      { false, 3, false } },
    { ActionState::rollStart, { false, 4, false } },
};

class Player : public cocos2d::Sprite
{
public:
    static Player* createPlayer();
    virtual bool init() override;

    // 核心更新方法：每帧调用，处理逻辑和坐标更新
    void update(float dt) override;

    // 属性
    CC_SYNTHESIZE(int, _health, Health);
    CC_SYNTHESIZE(int, _attack, Attack);
    CC_SYNTHESIZE(int, _defense, Defense);
    CC_SYNTHESIZE(float, _moveSpeed, MoveSpeed);
    CC_SYNTHESIZE(float, _jumpSpeed, JumpSpeed);
    CC_SYNTHESIZE(float, _rollSpeed, RollSpeed);
    CC_SYNTHESIZE(ActionState, _state, State);
    CC_SYNTHESIZE(MoveDirection, _direction, Direction);
    void changeState(ActionState newState);
    void playAnimation(ActionState state, bool loop);
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float delay);
    cocos2d::Animation* getAnimation(ActionState state);
    bool canChangeTo(ActionState newState);

private:
    cocos2d::Vec2 _velocity = cocos2d::Vec2::ZERO; // 用于跳跃和重力的速度向量
    std::unordered_map<ActionState, cocos2d::Animation*> _animationCache;

    Weapon* _mainWeapon;// 主武器
    Weapon* _subWeapon;// 副武器
};

#endif // __PLAYER_H__