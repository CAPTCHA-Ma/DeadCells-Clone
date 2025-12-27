#ifndef __DEADARCHER_H__
#define __DEADARCHER_H__

#include "Monster.h"
#include <unordered_map>

enum class DeadArcherState
{
    idle,
    walk,
    dead,
    shoot
};
static std::unordered_map<DeadArcherState, bool> DeadArcherStateLoop =
{
    { DeadArcherState::idle, true  },
    { DeadArcherState::walk,  true } ,
    { DeadArcherState::dead,  false },
    { DeadArcherState::shoot,false }
};
class DeadArcher : public Monster
{
public:

    CREATE_FUNC(DeadArcher);
    virtual bool init() override;

    virtual void idle() override;
    virtual void walk() override;
    virtual void onDead() override;
    virtual bool isAttackState() override { return _state == DeadArcherState::shoot; };
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) override;

    void shoot();
    void changeState(DeadArcherState newState);
    void playAnimation(DeadArcherState state, bool loop);
private:
    std::unordered_map<DeadArcherState, cocos2d::Animation*> _animCache;
    float _attackRange; // 攻击范围
    float _moveSpeed; // 水平移动速度
    cocos2d::Vec2 _velocity; // 用于跳跃和重力的速度向量
    cocos2d::Vec2 _lastPlayerPos; // 上次记录的玩家位置

    DeadArcherState _state;
    MoveDirection _direction;
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float time);
    cocos2d::Animation* getAnimation(DeadArcherState state);

    virtual void createAttackBox() override;

    float _aiTickTimer;

};

#endif // __DEADARCHER_H__
