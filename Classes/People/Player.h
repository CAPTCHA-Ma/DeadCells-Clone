#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include <unordered_map>
#include "WeaponNode.h"
#include "Monster.h"
#include "Weapon.h" 
#include "Arrow.h"

/**
 * @brief 动作状态配置结构体
 * * 用于定义特定状态的行为特征，如是否可被打断、优先级以及动画是否循环。
 */
struct StateConfig
{
    bool canBeInterrupted; // 当前状态是否可以被其他动作中断
    int priority;          // 状态优先级（数值越大，优先级越高）
    bool loop;             // 关联动画是否循环播放
};

/**
 * @brief 玩家动作状态枚举
 * * 涵盖了移动、战斗、特殊环境交互（爬墙）及受击死亡等所有逻辑状态。
 */
enum class ActionState
{
    idle,           // 待机
    walk,           // 行走
    run,            // 奔跑
    rollStart,      // 翻滚开始
    jumpDown,       // 下落
    jumpUp,         // 跳跃上升
    crouch,         // 下蹲
    dead,           // 死亡
    climbing,       // 攀爬中
    climbedge,      // 爬上边缘
    hanging,        // 悬挂/挂墙

    // 基础攻击
    atkA,
    atkB,

    // 武器专项攻击
    atkBackStabber,

    // 棒球棍系列连招
    AtkBaseballBatA,
    AtkBaseballBatB,
    AtkBaseballBatC,
    AtkBaseballBatD,
    AtkBaseballBatE,

    // 大剑系列连招
    atkBroadSwordA,
    atkBroadSwordB,
    atkBroadSwordC,

    // 烤箱斧系列连招
    AtkOvenAxeA,
    AtkOvenAxeB,
    AtkOvenAxeC,

    // 远程武器攻击
    closeCombatBow,
    dualBow,
    crossbowShoot,

    // 盾牌格挡结束动作
    blockEndLightningShield,
    blockEndParryShield,

    // 致命受击相关
    lethalHit,      // 致命一击受击
    lethalFall,     // 致命坠落
    lethalSlam,     // 致命砸地
};

/**
 * @brief 全局状态表
 * * 存储 ActionState 与其对应 StateConfig 的映射关系，用于状态机逻辑判定。
 */
static std::unordered_map<ActionState, StateConfig> StateTable =
{
    // 状态枚举                      能否被打断  优先级  是否循环
    { ActionState::idle,                { true,   0,   true  } },
    { ActionState::walk,                { true,   1,   true  } },
    { ActionState::run,                 { true,   1,   true  } },
    { ActionState::rollStart,           { true,   2,   false } },
    { ActionState::jumpDown,            { true,  97,   true  } },
    { ActionState::jumpUp,              { true,   2,   false } },
    { ActionState::crouch,              { true,   1,   true  } },
    { ActionState::dead,                { true,  100,  false } },
    { ActionState::climbing,            { true,  99,   true  } },
    { ActionState::climbedge,           { true,  99,   false } },
    { ActionState::hanging,             { true,  98,   false } },

    // 攻击动作
    { ActionState::atkA,                { true,   3,   false } },
    { ActionState::atkB,                { true,   3,   false } },
    { ActionState::atkBackStabber,      { true,   3,   false } },
    { ActionState::AtkBaseballBatA,     { true,   3,   false } },
    { ActionState::AtkBaseballBatB,     { true,   3,   false } },
    { ActionState::AtkBaseballBatC,     { true,   3,   false } },
    { ActionState::AtkBaseballBatD,     { true,   3,   false } },
    { ActionState::AtkBaseballBatE,     { true,   3,   false } },
    { ActionState::atkBroadSwordA,      { true,   3,   false } },
    { ActionState::atkBroadSwordB,      { true,   3,   false } },
    { ActionState::atkBroadSwordC,      { true,   3,   false } },
    { ActionState::AtkOvenAxeA,         { true,   3,   false } },
    { ActionState::AtkOvenAxeB,         { true,   3,   false } },
    { ActionState::AtkOvenAxeC,         { true,   3,   false } },
    { ActionState::closeCombatBow,      { true,   3,   false } },
    { ActionState::dualBow,             { true,   3,   false } },
    { ActionState::crossbowShoot,       { true,   3,   false } },

    // 格挡与受击
    { ActionState::blockEndLightningShield, { true,   3,   false } },
    { ActionState::blockEndParryShield,     { true,   3,   false } },
    { ActionState::lethalHit,           { true,  30,   false } },
    { ActionState::lethalFall,          { true,  99,   false } },
    { ActionState::lethalSlam,          { false, 99,   false } },
};

/**
 * @brief 玩家类
 * * 继承自 cocos2d::Sprite，负责处理玩家的物理、动画、战斗逻辑和状态管理。
 */
class Player : public cocos2d::Sprite
{
public:
    CREATE_FUNC(Player);
    bool init() override;

    // 物理与移动控制
    void changeDirection(MoveDirection dir);  // 改变精灵朝向
    void giveVelocityX(float speed);          // 赋予水平速度
    void giveVelocityY(float speed);          // 赋予垂直速度（爬墙用）
    void set0VelocityX();                     // 水平速度清零
    void set0VelocityY();                     // 垂直速度清零
    bool isOnGround() const;                  // 判定是否在地面

    // 状态查询
    bool isLethalState() const { return _state == ActionState::lethalHit || _state == ActionState::lethalFall || _state == ActionState::lethalSlam; };
    ActionState getCurrentState() const { return _state; };
    void update(float dt) override;

    // 动作接口
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

    // 武器与战斗逻辑
    void changeStateByWeapon(Weapon* weapon);     // 根据武器类型切换攻击状态
    void whenOnAttackKey(Weapon* w);              // 响应攻击按键
    void actionWhenEnding(ActionState state);     // 处理非循环动画结束时的回调
    void dead();                                  // 处理死亡逻辑
    void lethalHit();                             // 处理重击受击逻辑
    Weapon* getMainWeapon() const { return _mainWeapon; };
    void swapWeapon();                            // 切换主副武器
    void getNewWeapon(Weapon* newWeapon);         // 装备新武器

    // 状态机与动画核心
    void changeState(ActionState newState);       // 尝试切换到新状态
    bool whetherCanChangeToNewState(ActionState newState) const; // 检查状态切换合法性
    void playAnimation(ActionState state, bool loop); // 播放状态对应动画

    bool gameEnding = false;                      // 游戏结束标志

    // 攻击检测系统
    Weapon* _currentAttackingWeapon = nullptr;    // 记录当前发起攻击的武器指针
    float getFinalAttack() const;                 // 获取总攻击力（基础+武器）
    void struck(float attackPower);               // 处理被怪物攻击
    void shootArrow();                            // 远程发射逻辑

    // 属性与 UI
    bool isInvincible() const { return _invincible; }; // 是否处于无敌状态
    void setupHPBar();                            // 初始化血条 UI
    void updateHPBar();                           // 更新血条显示
    bool isParrying() const { return _isParrying; } // 是否处于招架判定中

protected:
    // 自动生成的属性访问器
    CC_SYNTHESIZE(BasicAttributes, _currentAttributes, CurrentAttributes); // 最终属性
    CC_SYNTHESIZE(float, _maxHealth, MaxHealth);                         // 最大生命值

    // 移动相关参数
    float _runSpeed;
    float _rollSpeed;
    float _jumpSpeed;
    float _climbSpeed;

    // 状态与方向
    ActionState _state;
    MoveDirection _direction;
    UpDownDirection _directionY = UpDownDirection::NONE;
    cocos2d::Vec2 _velocity;

    // 装备系统
    Weapon* _mainWeapon;
    Weapon* _subWeapon;

    // 连招系统逻辑
    bool isAttackState(ActionState s) const;      // 判断给定状态是否属于攻击动作
    bool _comboInput;                             // 连招输入缓存标志
    bool _canCombo;                               // 连招窗口是否开启
    int _comboStep = 0;                           // 当前处于连招的第几段

    // UI 组件
    void setupGoldLabel();
    void updateGoldDisplay(int totalGold);

private:
    cocos2d::Label* _goldLabelNode = nullptr;     // 金币显示文本

    // 动画底层处理
    bool _invincible = false;                     // 无敌标志位
    cocos2d::DrawNode* _hpBarNode = nullptr;      // 绘图节点实现的血条
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float delay) const;
    cocos2d::Animation* getAnimation(ActionState state);
    std::unordered_map<ActionState, cocos2d::Animation*> _animationCache; // 动画缓存

    // 物理碰撞检测辅助
    cocos2d::Node* _attackNode = nullptr;         // 攻击检测盒容器
    cocos2d::Node* _hurtNode = nullptr;           // 受击检测
    std::set<Monster*> _hitMonsters;              // 单次攻击内已击中的怪物列表（防止多帧重复伤害）

    // 物理刚体构建
    void updatePhysicsBody(const cocos2d::Size& size, const cocos2d::Vec2& offset);
    void setupBodyProperties(cocos2d::PhysicsBody* body);
    void createNormalBody();                      // 创建标准受击框
    void createRollBody();                        // 创建翻滚受击框
    void startRollInvincible(float time);         // 开启限时无敌
    void createRollBox();                         // 创建翻滚时的逻辑处理
    void createAttackBox();                       // 创建近战伤害判定区
    void createShieldParryBox();                  // 创建招架判定区
    void removeAttackBox();                       // 移除判定区

    bool _isParrying = false;                     // 是否处于瞬间格挡/招架状态
};

#endif // __PLAYER_H__