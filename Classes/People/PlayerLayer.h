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
    bool isInvincible() const { return _player->isInvincible(); };
    bool _isDropping = false, _isBelowLadder = false, _isAboveLadder = false, _isContactBottom = false, _isPassingPlatform = false;
    float getFinalAttack() const{ return _player->getFinalAttack(); };
    ActionState getCurrentState() const { return _player->_state; };
    cocos2d::Vec2 GetVelo();
    void setNearbyWeapon(WeaponNode* weaponNode) { _nearbyWeapon = weaponNode; };
    WeaponNode* getNearbyWeapon() const { return _nearbyWeapon; };
    void getNewWeapon();
    bool gameEnding() const { return _player->gameEnding; };

    void recordMonsterHit(Monster* monster) {if (monster) _hitMonsters.insert(monster); }
    bool isMonsterAlreadyHit(Monster* monster) {return _hitMonsters.find(monster) != _hitMonsters.end(); }
    void clearHitMonsters() {_hitMonsters.clear(); }

private:
  
    Player* _player;
    bool _leftPressed = false;
    bool _rightPressed = false;
	bool _downPressed = false;
    void setupEventListeners();
    WeaponNode* _nearbyWeapon = nullptr;
    std::set<Monster*> _hitMonsters; // ´ò»÷Ãûµ¥

};

#endif // __PLAYER_LAYER_H__