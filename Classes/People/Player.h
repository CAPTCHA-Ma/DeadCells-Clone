// Player.h
#ifndef __PLAYER_H__
#define __PLAYER_H__
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
    idle,
    run,        // 奔跑
    rollStart,
    jumpDown,//
    jumpUp,
    crouch,     // 下蹲
    atkA,   // 徒手攻击 (或通用攻击)
    atkB,
    atkBackStabber,
    AtkBaseballBatA,
    AtkBaseballBatB,
    AtkBaseballBatC,
    AtkBaseballBatD,
    AtkBaseballBatE,
    atkBroadSwordA,
    AtkcloseCombatBow,
    AtkdualBow,
    crossbowShoot,
    dead
};

static std::unordered_map<ActionState, StateConfig> StateTable =
{//                     能否被打断 优先级数字越大优先级越高   动画是否循环播放
    { ActionState::idle,                                   { true,  0, true  } },
    { ActionState::run,                                     { true,  1, true  } },
    { ActionState::rollStart,                               { true,  4, false } },
    { ActionState::jumpDown,                                { false,99, true } },
    { ActionState::jumpUp,                                  { true,  2, false } },
    { ActionState::crouch,                                  { true,  0, true } },
    { ActionState::atkA,                                    { false, 3, false } },
    { ActionState::atkB,                                    { false, 3, false } },
    { ActionState::atkBackStabber,                          { false, 3, false } },
    { ActionState::AtkBaseballBatA,                         { false, 3, false } },
    { ActionState::AtkBaseballBatB,                         { false, 3, false } },
    { ActionState::AtkBaseballBatC,                         { false, 3, false } },
    { ActionState::AtkBaseballBatD,                         { false, 3, false } },
    { ActionState::AtkBaseballBatE,                         { false, 3, false } },
    { ActionState::atkBroadSwordA,                          { false, 3, false } },
    { ActionState::AtkcloseCombatBow,                       { false, 3, false } },
    { ActionState::AtkdualBow,                              { false, 3, false } },
    { ActionState::crossbowShoot,                           { false, 3, false } },
};

class Player : public cocos2d::Sprite
{
public:

    CREATE_FUNC(Player);
    bool Player::init() override;
    //动作
	void changeDirection(MoveDirection dir);
    void idle();
    void jumpUp();
    void run();
    void rollStart();
    void jumpDown();
    void crouch();
    void atkA();
    void atkB();
    void atkBackStabber();
    void AtkBaseballBatA();
    void AtkBaseballBatB();
    void AtkBaseballBatC();
    void AtkBaseballBatD();
    void AtkBaseballBatE();
    void atkBroadSwordA();
    void AtkcloseCombatBow();
    void AtkdualBow();
    void crossbowShoot();
    void changeStateByWeapon(Weapon* weapon);
    void whenOnAttackKey(Weapon* w);
    void actionWhenEnding(ActionState state);
    void dead();

    //动画
    void changeState(ActionState newState);
	bool whetherCanChangeToNewState(ActionState newState) const;

	//攻击及武器系统
    void getWeapon(Weapon* w);
    void struck(int value);
    



protected:
	CC_SYNTHESIZE(BasicAttributes, _originalAttributes, OriginalAttributes);//初始属性只与等级有关
	CC_SYNTHESIZE(BasicAttributes, _finalAttributes, FinalAttributes);//最终属性与装备有关
	CC_SYNTHESIZE(int, _level, Level);
	float _moveSpeed; // 水平移动速度
	float _rollSpeed; // 滚动速度
	float _jumpSpeed; // 跳跃初速度
    ActionState _state;
	MoveDirection _direction;
    cocos2d::Vec2 _velocity; // 用于跳跃和重力的速度向量
    Weapon* _mainWeapon;// 主武器
    Weapon* _subWeapon;// 副武器


    bool isAttackState(ActionState s) const;    //判断是否为攻击状态
    bool _comboInput;                           // 是否缓存了输入
    bool _canCombo;                             // 当前是否允许连招
	int _comboStep = 0;                         // 当前连招步骤


private:
    //动画
    void playAnimation(ActionState state, bool loop);
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float delay) const;
    cocos2d::Animation* getAnimation(ActionState state);
    std::unordered_map<ActionState, cocos2d::Animation*> _animationCache;

	//攻击判定
    cocos2d::Node* _attackNode = nullptr;
    cocos2d::Node* _hurtNode = nullptr;
    void createHurtBox();
    void createAttackBox();
    void removeAttackBox();


};

#endif // __PLAYER_H__