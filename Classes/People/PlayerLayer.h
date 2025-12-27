#ifndef __PLAYER_LAYER_H__
#define __PLAYER_LAYER_H__

#include "cocos2d.h"
#include "Player.h"
#include "WeaponNode.h"
#include "Monster.h"
#include <set>

class PlayerLayer : public cocos2d::Layer
{
public:
    static PlayerLayer* create(cocos2d::Vec2 pos = cocos2d::Vec2(200, 200));
    virtual bool init(cocos2d::Vec2 pos);
    virtual void update(float dt) override;

    // 状态与属性获取
    cocos2d::Vec2 getPlayerWorldPosition() const;
    cocos2d::Vec2 GetVelo();
    ActionState getCurrentState() const { return _player->_state; }
    float getFinalAttack() const { return _player->getFinalAttack(); }
    bool isInvincible() const { return _player->isInvincible(); }
    bool gameEnding() const { return _player->gameEnding; }

    // 战斗逻辑
    void struck(float attackPower, cocos2d::Vec2 sourcePos);

    // 打击名单管理
    void recordMonsterHit(Monster* monster);
    bool isMonsterAlreadyHit(Monster* monster);
    void clearHitMonsters();

    // 武器与金币逻辑
    void setNearbyWeapon(WeaponNode* weaponNode) { _nearbyWeapon = weaponNode; }
    WeaponNode* getNearbyWeapon() const { return _nearbyWeapon; }
    void getNewWeapon();
    int getGold() const { return _gold; }
    void addGold(int amount) { _gold += amount; }
    bool spendGold(int amount);

    // 梯子与平台物理状态标识
    bool _isDropping = false;
    bool _isBelowLadder = false;
    bool _isAboveLadder = false;
    bool _isContactBottom = false;
    bool _isPassingPlatform = false;

private:
    PlayerLayer() = default;
    ~PlayerLayer() = default;

    void setupEventListeners();

    Player* _player = nullptr;
    WeaponNode* _nearbyWeapon = nullptr;
    cocos2d::Sprite* _pickupTip = nullptr;

    bool _leftPressed = false;
    bool _rightPressed = false;
    bool _downPressed = false;
    int _gold = 0;

    // 存储本轮攻击已经命中的怪物名单
    std::set<Monster*> _hitMonsters;
};

#endif // __PLAYER_LAYER_H__