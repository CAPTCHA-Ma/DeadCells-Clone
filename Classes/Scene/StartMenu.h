#pragma once
#include "cocos2d.h"

class StartMenu : public cocos2d::Layer
{
public:
    virtual bool init() override;

    virtual void onEnter() override;

    static cocos2d::Scene* createSceneWithPhysics();
    CREATE_FUNC(StartMenu);
};