#ifndef __PLAYER_LAYER_H__
#define __PLAYER_LAYER_H__
#include "Player.h"

class PlayerLayer : public cocos2d::Layer
{
public:
    bool init(cocos2d::Vec2 pos);
    cocos2d::Vec2 getPlayerWorldPosition() const;
    static PlayerLayer* create(cocos2d::Vec2 pos = cocos2d::Vec2(200,200));
    void update(float dt);
    void struck(float attackPower);
    float getFinalAttack() { return _player->getFinalAttributes().attack; };
    bool isInvincible() {return _player->_invincible; };
private:
    Player* _player;
    bool _leftPressed = false;
    bool _rightPressed = false;
    void setupEventListeners();
};

#endif // __PLAYER_LAYER_H__
