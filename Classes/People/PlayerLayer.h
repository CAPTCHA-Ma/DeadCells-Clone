#ifndef __PLAYER_LAYER_H__
#define __PLAYER_LAYER_H__

#include "cocos2d.h"
#include "Player.h"
#include "WeaponNode.h"
#include "Monster.h"
#include <set>

/**
 * PlayerLayer 类：玩家管理层
 * 职责：连接输入系统、武器系统、经济系统与玩家物理表现
 */
class PlayerLayer : public cocos2d::Layer
{
public:
    static PlayerLayer* create(cocos2d::Vec2 pos = cocos2d::Vec2(200, 200));
    virtual bool init(cocos2d::Vec2 pos);
    virtual void update(float dt) override;

    // --- 状态与属性查询 ---
    cocos2d::Vec2 getPlayerWorldPosition() const; // 获取玩家在世界空间中的坐标（供怪物 AI 追踪使用）
    cocos2d::Vec2 GetVelo();                       // 获取当前物理速度
    ActionState getCurrentState() const { return _player->_state; }
    float getFinalAttack() const { return _player->getFinalAttack(); }
    bool isInvincible() const { return _player->isInvincible(); } // 查询是否处于无敌帧（受击后）
    bool gameEnding() const { return _player->gameEnding; }       // 查询游戏是否结束
    void healthUp(int value);                                     // 回血逻辑

    // --- 战斗逻辑核心 ---
    /**
     * 玩家受到攻击
     * @param attackPower 伤害数值
     * @param sourcePos 伤害来源位置（用于计算受击击退方向）
     */
    void struck(float attackPower, cocos2d::Vec2 sourcePos);

    // --- 打击名单管理（防止多段伤害判定） ---
    void recordMonsterHit(Monster* monster);      // 记录本次挥剑已命中的怪物
    bool isMonsterAlreadyHit(Monster* monster);   // 检查该怪物是否在本次攻击中已受过伤
    void clearHitMonsters();                      // 攻击动作结束或开始新攻击时重置

    // --- 交互与经济逻辑 ---
    void setNearbyWeapon(WeaponNode* weaponNode) { _nearbyWeapon = weaponNode; }
    WeaponNode* getNearbyWeapon() const { return _nearbyWeapon; } // 获取脚下可捡起的武器
    void getNewWeapon();                          // 捡起武器动作
    int getGold() const { return _gold; }         // 查询金币
    void addGold(int amount);                     // 增加金币
    bool reduceGold(int amount);                  // 尝试扣除金币（检查是否足够）
    bool spendGold(int amount);                   // 消费逻辑

    // --- 梯子与平台物理状态标识 (关键用于控制单向穿透和攀爬) ---
    bool _isDropping = false;        // 是否正在从平台向下跳
    bool _isBelowLadder = false;     // 是否在梯子底部
    bool _isAboveLadder = false;     // 是否在梯子顶部
    bool _isContactBottom = false;   // 是否接触到地面边界
    bool _isPassingPlatform = false; // 是否正在穿过单向平台

private:
    PlayerLayer() = default;
    ~PlayerLayer() = default;
    void setupEventListeners(); // 设置键盘或触摸监听

    Player* _player = nullptr;           // 真正的玩家显示对象与物理对象
    WeaponNode* _nearbyWeapon = nullptr; // 当前地面上距离玩家最近的武器节点
    cocos2d::Sprite* _pickupTip = nullptr; // “按 E 拾取”之类的提示 UI

    // --- 移动与控制变量 ---
    int _jumpCount = 0;              // 当前跳跃次数（用于二段跳判定）
    const int _maxJumpCount = 2;     // 最大跳跃次数限制
    bool _leftPressed = false;
    bool _rightPressed = false;
    bool _downPressed = false;
    int _gold = 0;                   // 玩家持有的金币总量

    // 存储本轮攻击已经命中的怪物名单（防止一帧产生多次伤害）
    std::set<Monster*> _hitMonsters;
};

#endif // __PLAYER_LAYER_H__