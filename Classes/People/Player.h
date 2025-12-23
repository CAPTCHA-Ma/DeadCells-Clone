// Player.h
#ifndef __PLAYER_H__
#define __PLAYER_H__
#include <unordered_map>
#include "Weapon.h" 
#include "FlyingObject.h"
#include "Arrow.h"
#include "Bomb.h"
struct StateConfig
{
    bool canBeInterrupted;
    int priority;
    bool loop;
};

enum class ActionState 
{
    idle,
    walk,
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
    atkBroadSwordB,
    atkBroadSwordC,


    AtkOvenAxeA,
    AtkOvenAxeB,
    AtkOvenAxeC,

    AtkcloseCombatBow,
    AtkdualBow,
    crossbowShoot,

    blockEndLightningShield,
    blockEndParryShield,

    lethalFall,
    lethalSlam,

};

static std::unordered_map<ActionState, StateConfig> StateTable =
{//                     能否被打断 优先级数字越大优先级越高   动画是否循环播放
    { ActionState::idle,                                    { true,  0, true  } },
    { ActionState::walk,                                    { true,  1, true  } },
    { ActionState::run,                                     { true,  1, true  } },
    { ActionState::rollStart,                               { true,  2, false } },
    { ActionState::jumpDown,                                { true, 99, true } },
    { ActionState::jumpUp,                                  { true,  2, false } },
    { ActionState::crouch,                                  { true,  1, true } },


    //攻击
    { ActionState::atkA,                                    { false, 3, false } },
    { ActionState::atkB,                                    { false, 3, false } },


    { ActionState::atkBackStabber,                          { false, 3, false } },

    { ActionState::AtkBaseballBatA,                         { false, 3, false } },
    { ActionState::AtkBaseballBatB,                         { false, 3, false } },
    { ActionState::AtkBaseballBatC,                         { false, 3, false } },
    { ActionState::AtkBaseballBatD,                         { false, 3, false } },
    { ActionState::AtkBaseballBatE,                         { false, 3, false } },


    { ActionState::atkBroadSwordA,                          { false, 3, false } },
    { ActionState::atkBroadSwordB,                          { false, 3, false } },
    { ActionState::atkBroadSwordC,                          { false, 3, false } },

    { ActionState::AtkOvenAxeA,                             { false, 3, false } },
    { ActionState::AtkOvenAxeB,                             { false, 3, false } },
    { ActionState::AtkOvenAxeC,                             { false, 3, false } },

    { ActionState::AtkcloseCombatBow,                       { false, 3, false } },
    { ActionState::AtkdualBow,                              { false, 3, false } },
    { ActionState::crossbowShoot,                           { false, 3, false } },

    { ActionState::blockEndLightningShield,                 { false, 3, false } },
    { ActionState::blockEndParryShield,                     { false, 3, false } },

    { ActionState::lethalFall,                              { false, 99, false } },
    { ActionState::lethalSlam,                              { false, 99, false } },
};

class Player : public cocos2d::Sprite
{
public:

    CREATE_FUNC(Player);
    bool Player::init() override;
    void changeDirection(MoveDirection dir);
	void giveVelocityX(float speed);
	void set0VelocityX();
	void set0VelocityY();
    bool isOnGround() const;
	void update(float dt);

    //动作
    void idle();
    void walk();
    void run();
    void rollStart();
    void jumpUp();
    void jumpDown();
    void crouch();
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
    void playAnimation(ActionState state, bool loop);

	//攻击及武器系统
    void getWeapon(Weapon* w);
    void struck(int value);
    void shootArrow();
    void throwBomb();



protected:
	CC_SYNTHESIZE(BasicAttributes, _originalAttributes, OriginalAttributes);//初始属性只与等级有关
	CC_SYNTHESIZE(BasicAttributes, _finalAttributes, FinalAttributes);//最终属性与装备有关
	CC_SYNTHESIZE(int, _level, Level);

	float _runSpeed; // 水平移动速度
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
   
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float delay) const;
    cocos2d::Animation* getAnimation(ActionState state);
    std::unordered_map<ActionState, cocos2d::Animation*> _animationCache;

	//攻击判定
    cocos2d::Node* _attackNode = nullptr;
    cocos2d::Node* _hurtNode = nullptr;
    void createHurtBox();
    void createRollBox();
    void createAttackBox();
    void createBlockEndBox();
    void removeHurtBox();
    void removeAttackBox();


};

#endif // __PLAYER_H__