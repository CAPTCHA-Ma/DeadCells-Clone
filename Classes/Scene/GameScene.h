#pragma once
#include "cocos2d.h"
#include "Map\SceneMap.h"
#include "People\Monster.h"
#include "People\Player.h"

class GameScene : public cocos2d::Scene
{
public:
    SceneMap* scenemap = nullptr; 
    SceneMapData* SceneMapData = nullptr; 

    virtual bool init() override; 
    CREATE_FUNC(GameScene);
};