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
    float getFinalAttack() { return _player->getFinalAttributes().attack; };
    bool isInvincible() { return _player->_invincible; };
    bool _isDropping = false, _isBelowLadder = false, _isAboveLadder = false, _isContactBottom = false, _isPassingPlatform = false;
    ActionState getCurrentState() const { return _player->_state; };
    cocos2d::Vec2 GetVelo();

private:
    cocos2d::Sprite* _pickupTip = nullptr; // 捡取提示图标
    Player* _player;
    bool _leftPressed = false;
    bool _rightPressed = false;
	bool _downPressed = false;
    void setupEventListeners();
};

#endif // __PLAYER_LAYER_H__