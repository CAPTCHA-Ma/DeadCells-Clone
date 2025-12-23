#ifndef __GRENADIER_H__
#define __GRENADIER_H__

#include "Monster.h"
#include <unordered_map>

enum class GrenadierState
{
    idle,
    atk,
    walk,
    dead
};
static std::unordered_map<GrenadierState, bool> GrenadierStateLoop =
{
    { GrenadierState::idle, true  },
    { GrenadierState::atk,  false },
    { GrenadierState::walk,  true } ,
    { GrenadierState::dead,  false }
};
class Grenadier : public Monster
{
public:

    CREATE_FUNC(Grenadier);
    virtual bool init() override;

	virtual void idle() override;
    void attack();
    virtual void walk() override;
    virtual void onDead() override;
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) override;
    void changeState(GrenadierState newState);
    void playAnimation(GrenadierState state, bool loop);
private:
    std::unordered_map<GrenadierState, cocos2d::Animation*> _animCache;
	float _attackRange; // 攻击范围
    float _moveSpeed; // 水平移动速度
    cocos2d::Vec2 _velocity; // 用于跳跃和重力的速度向量
	cocos2d::Vec2 _lastPlayerPos; // 上次记录的玩家位置

    GrenadierState _state;
    MoveDirection _direction;
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float time);
    cocos2d::Animation* getAnimation(GrenadierState state);

	virtual void createHurtBox() override;
    virtual void createAttackBox() override;

	float _aiTickTimer;

};

#endif // __GRENADIER_H__
