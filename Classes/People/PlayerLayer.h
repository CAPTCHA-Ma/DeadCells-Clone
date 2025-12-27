#ifndef __PLAYER_LAYER_H__
#define __PLAYER_LAYER_H__
#include "Player.h"

class PlayerLayer : public cocos2d::Layer
{
public:
    bool init(cocos2d::Vec2 pos);
    cocos2d::Vec2 getPlayerWorldPosition() const;
    static PlayerLayer* create(cocos2d::Vec2 pos = cocos2d::Vec2(200, 200));
    void update(float dt);
    void struck(float attackPower, cocos2d::Vec2 sourcePos);
    float getFinalAttack() const { return _player->getFinalAttributes().attack; };
    bool isInvincible() const { return _player->_invincible; };
    bool _isDropping = false, _isBelowLadder = false, _isAboveLadder = false, _isContactBottom = false, _isPassingPlatform = false;
    ActionState getCurrentState() const { return _player->_state; };
    cocos2d::Vec2 GetVelo();
    void setNearbyWeapon(WeaponNode* weaponNode) { _nearbyWeapon = weaponNode; };
    WeaponNode* getNearbyWeapon() const { return _nearbyWeapon; };
    void getNewWeapon();

    int getGold() const { return _gold; }
    void addGold(int amount) { _gold += amount; }
    bool spendGold(int amount) {
        if (_gold >= amount) {
            _gold -= amount;
            return true;
        }
        return false;
    }

private:
    cocos2d::Sprite* _pickupTip = nullptr;
    Player* _player;
    bool _leftPressed = false;
    bool _rightPressed = false;
	bool _downPressed = false;
    void setupEventListeners();
    WeaponNode* _nearbyWeapon = nullptr;
    int _gold = 0;

};

#endif // __PLAYER_LAYER_H__