#ifndef __PLAYER_LAYER_H__
#define __PLAYER_LAYER_H__
#include "Player.h"

class PlayerLayer : public cocos2d::Layer
{
public:
    virtual bool init() override;
    CREATE_FUNC(PlayerLayer);
    void update(float dt);
private:
    Player* _player;
    bool _leftPressed = false;
    bool _rightPressed = false;
    void setupEventListeners();
};

#endif // __PLAYER_LAYER_H__
