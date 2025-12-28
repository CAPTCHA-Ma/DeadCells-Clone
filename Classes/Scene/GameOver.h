#pragma once
#include "cocos2d.h"
#include "Res/strings.h"
#include "StartMenu.h"
#include "ui/CocosGUI.h"
#include "GameScene.h"
#include "Prison.h"

// 游戏结束场景类，显示游戏结束选项
class GameOver : public cocos2d::Scene
{
public:

    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameOver);

};