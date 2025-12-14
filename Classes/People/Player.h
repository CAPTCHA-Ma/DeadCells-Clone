// Player.h
#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "cocos2d.h"
#include <unordered_map>
#include "Weapon.h" 
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
    rollStart,
    hang,//悬空
    jumpUp,
    crouch,     // 下蹲
    atkA,   // 徒手攻击 (或通用攻击)
    atkB,
    AtkAlucardSwordA,
    atkBackStabber,
    AtkBaseballBatA,
    atkBroadSword

};
enum class MoveDirection 
{
    LEFT,
    RIGHT
};
static std::unordered_map<ActionState, StateConfig> StateTable =
{//                     能否被打断 优先级数字越大优先级越高   动画是否循环播放
    { ActionState::stand,                                   { true,  0, true  } },
    { ActionState::run,                                     { true,  1, true  } },
    { ActionState::rollStart,                               { true,  4, false } },
    { ActionState::hang,                                    { false,99, true } },
    { ActionState::jumpUp,                                  { true,  2, false } },
    { ActionState::crouch,                                  { true,  0, true } },
    { ActionState::atkA,                                    { false, 3, false } },
    { ActionState::atkB,                                    { false, 3, false } },
    { ActionState::AtkAlucardSwordA,                        { false, 3, false } },
};

class Player : public cocos2d::Sprite
{
public:
    CREATE_FUNC(Player);
    bool Player::init() override;
    void changeState(ActionState newState);
    void playAnimation(ActionState state, bool loop);
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float delay);
    cocos2d::Animation* getAnimation(ActionState state);
    bool canChangeTo(ActionState newState);
    void stand();
    void jumpUp();
    void run();
	void rollStart();
    void hang();
	void crouch();
    void atkA();   
    void atkB();
	void AltackAlucardSwordA();
	void atkBackStabber();
	void AtkBaseballBatA();
	void atkBroadSword();
    void atkWithWeapon(Weapon* weapon);
protected:
    CC_SYNTHESIZE(int, _health, Health);
    CC_SYNTHESIZE(int, _attack, Attack);
    CC_SYNTHESIZE(int, _defense, Defense);
	float _moveSpeed = 150.0f; // 水平移动速度
	float _rollSpeed = 300.0f; // 滚动速度
	float _jumpSpeed = 300.0f; // 跳跃初速度
    ActionState _state = ActionState::stand;
	MoveDirection _direction = MoveDirection::RIGHT;
    cocos2d::Vec2 _velocity = cocos2d::Vec2::ZERO; // 用于跳跃和重力的速度向量
    Weapon* _mainWeapon;// 主武器
    Weapon* _subWeapon;// 副武器
private:
    std::unordered_map<ActionState, cocos2d::Animation*> _animationCache;
};

#endif // __PLAYER_H__