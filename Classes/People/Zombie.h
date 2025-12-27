#ifndef __ZOMBIE_H__
#define __ZOMBIE_H__

#include "Monster.h"
#include <unordered_map>

enum class ZombieState
{
    idle,
    atkA,
    walk,
    run,
    dead
};
static std::unordered_map<ZombieState, bool> ZombieStateLoop =
{
    { ZombieState::idle, true  },
    { ZombieState::atkA,  false },
    { ZombieState::walk,  true },
    { ZombieState::run,  true },
    { ZombieState::dead,  false }
};
class Zombie : public Monster
{
public:

    CREATE_FUNC(Zombie);
    virtual bool init() override;
    virtual void idle() override;
    virtual void onDead() override;
    virtual bool isAttackState() override { return _state == ZombieState::atkA; };
    void atkA();
    virtual void walk() override;
    void run();
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) override;
    void changeState(ZombieState newState);
    void playAnimation(ZombieState state, bool loop);


private:
    std::unordered_map<ZombieState, cocos2d::Animation*> _animCache;
    float _attackRange; // 攻击范围
    float _moveSpeed; // 水平移动速度
	float _runSpeed; // 冲刺速度
    cocos2d::Vec2 _velocity; // 用于跳跃和重力的速度向量


    ZombieState _state;
    MoveDirection _direction;
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float time);
    cocos2d::Animation* getAnimation(ZombieState state);

    virtual void createAttackBox() override;

    float _aiTickTimer;

};

#endif // __GRENADIER_H__
