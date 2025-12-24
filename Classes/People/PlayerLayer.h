#ifndef __PLAYER_LAYER_H__
#define __PLAYER_LAYER_H__
#include "Player.h"

class PlayerLayer : public cocos2d::Layer
{
public:
    bool init(cocos2d::Vec2 pos);
    static PlayerLayer* create(cocos2d::Vec2 pos = cocos2d::Vec2(200,200));
    void update(float dt);
    bool _isDropping = false;

private:
    Player* _player;
    bool _leftPressed = false;
    bool _rightPressed = false;
	bool _downPressed = false; 
    void setupEventListeners();
};

#endif // __PLAYER_LAYER_H__
