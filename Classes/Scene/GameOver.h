#pragma once
#include "cocos2d.h"
#include "Res/strings.h"
#include "StartMenu.h"
#include "ui/CocosGUI.h"
#include "GameScene.h"
#include "Prison.h"

class GameOver : public cocos2d::Scene
{
public:

    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameOver);

};