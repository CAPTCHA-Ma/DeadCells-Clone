#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "cocos2d.h"
#include <unordered_map>
#include "WeaponNode.h"
#include "Monster.h"
#include "Weapon.h" 
#include "Arrow.h"
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
    dead,
    climbing,
    climbedge,
    hanging,

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



    closeCombatBow,
    dualBow,
    crossbowShoot,

    blockEndLightningShield,
    blockEndParryShield,

    lethalHit,
    lethalFall,
    lethalSlam,

};

static std::unordered_map<ActionState, StateConfig> StateTable =
{//                     能否被打断 优先级数字越大优先级越高   动画是否循环播放
    { ActionState::idle,                                    { true,  0, true  } },
    { ActionState::walk,                                    { true,  1, true  } },
    { ActionState::run,                                     { true,  1, true  } },
    { ActionState::rollStart,                               { true,  2, false } },
    { ActionState::jumpDown,                                { true, 97, true } },
    { ActionState::jumpUp,                                  { true,  2, false } },
    { ActionState::crouch,                                  { true,  1, true } },
    { ActionState::dead,                                    { true,  100, false }},
	{ ActionState::climbing,                                 { true,  99, true  } },
	{ ActionState::climbedge,                               { true,  99, false  } },
	{ ActionState::hanging,                                 { true,  98, false  } },

    //攻击
    { ActionState::atkA,                                    { true, 3, false } },
    { ActionState::atkB,                                    { true, 3, false } },


    { ActionState::atkBackStabber,                          { true, 3, false } },

    { ActionState::AtkBaseballBatA,                         { true, 3, false } },
    { ActionState::AtkBaseballBatB,                         { true, 3, false } },
    { ActionState::AtkBaseballBatC,                         { true, 3, false } },
    { ActionState::AtkBaseballBatD,                         { true, 3, false } },
    { ActionState::AtkBaseballBatE,                         { true, 3, false } },


    { ActionState::atkBroadSwordA,                          { true, 3, false } },
    { ActionState::atkBroadSwordB,                          { true, 3, false } },
    { ActionState::atkBroadSwordC,                          { true, 3, false } },

    { ActionState::AtkOvenAxeA,                             { true, 3, false } },
    { ActionState::AtkOvenAxeB,                             { true, 3, false } },
    { ActionState::AtkOvenAxeC,                             { true, 3, false } },

    { ActionState::closeCombatBow,                         { true, 3, false } },
    { ActionState::dualBow,                                { true, 3, false } },
    { ActionState::crossbowShoot,                           { true, 3, false } },

    { ActionState::blockEndLightningShield,                 { true, 3, false } },
    { ActionState::blockEndParryShield,                     { true, 3, false } },

    { ActionState::lethalHit,                               { true, 30, false } },
    { ActionState::lethalFall,                              { true, 99, false } },
    { ActionState::lethalSlam,                              { false, 99, false } },
};

class Player : public cocos2d::Sprite
{
public:

    CREATE_FUNC(Player);
    bool Player::init() override;
    void changeDirection(MoveDirection dir);//改变方向
	void giveVelocityX(float speed);
	void giveVelocityY(float speed);
	void set0VelocityX();//X速度置0
	void set0VelocityY();//Y速度置0
    bool isOnGround() const;
    bool isLethalState() const { return _state == ActionState::lethalHit || _state == ActionState::lethalFall || _state == ActionState::lethalSlam; };
    ActionState getCurrentState() const { return _state; };
	void update(float dt);

    //动作
    void idle();
    void walk();
    void run();
    void rollStart();
    void jumpUp();
    void jumpDown();
    void crouch();
	void hanging();
    void climbing();
    void climbedge();
    void closeCombatBow();
    void dualBow();
    void crossbowShoot();
    void changeStateByWeapon(Weapon* weapon);
    void whenOnAttackKey(Weapon* w);
    void actionWhenEnding(ActionState state);
    void dead();
    void lethalHit();
    Weapon* getMainWeapon() const { return _mainWeapon; };
    void swapWeapon();
    void getNewWeapon(Weapon* newWeapon);
    //动画
    void changeState(ActionState newState);
	bool whetherCanChangeToNewState(ActionState newState) const;
    void playAnimation(ActionState state, bool loop);
    bool gameEnding = false;
	//攻击及武器系统
    Weapon* _currentAttackingWeapon = nullptr; // 记录当前发起攻击的武器
    float getFinalAttack() const;
    void struck(float attackPower);
    void shootArrow() ;

    bool isInvincible() const { return _invincible; };
    void setupHPBar();
    void updateHPBar();
    bool isParrying() const { return _isParrying; }
protected:

	CC_SYNTHESIZE(BasicAttributes, _currentAttributes, CurrentAttributes);//最终属性与装备有关
    CC_SYNTHESIZE(float, _maxHealth, MaxHealth);//最大生命值
    
	float _runSpeed; // 水平移动速度
	float _rollSpeed; // 滚动速度
	float _jumpSpeed; // 跳跃初速度
	float _climbSpeed; // 爬升速度
    ActionState _state;
    MoveDirection _direction;
    UpDownDirection _directionY = UpDownDirection::NONE;
    cocos2d::Vec2 _velocity; // 用于跳跃和重力的速度向量
    Weapon* _mainWeapon;// 主武器
    Weapon* _subWeapon;// 副武器


    bool isAttackState(ActionState s) const;    //判断是否为攻击状态
    bool _comboInput;                           // 是否缓存了输入
    bool _canCombo;                             // 当前是否允许连招
	int _comboStep = 0;                         // 当前连招步骤

    void setupGoldLabel();
    void updateGoldDisplay(int totalGold);
private:
    cocos2d::Label* _goldLabelNode = nullptr; // 用于显示金币的文本节点
    //动画
    bool _invincible = false;
    cocos2d::DrawNode* _hpBarNode = nullptr;
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float delay) const;
    cocos2d::Animation* getAnimation(ActionState state);
    std::unordered_map<ActionState, cocos2d::Animation*> _animationCache;

	//攻击判定
    cocos2d::Node* _attackNode = nullptr;
    cocos2d::Node* _hurtNode = nullptr;
    std::set<Monster*> _hitMonsters; 

    void updatePhysicsBody(const cocos2d::Size& size, const cocos2d::Vec2& offset);
    void setupBodyProperties(cocos2d::PhysicsBody* body);
    void createNormalBody();
    void createRollBody();
    void startRollInvincible(float time);
    void createRollBox();
    void createAttackBox();
    void createShieldParryBox();
    void removeAttackBox();
    bool _isParrying = false; // 是否处于格挡判定中
};

#endif // __PLAYER_H__