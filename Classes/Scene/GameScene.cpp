#include "GameScene.h"
#include "Res/strings.h"

USING_NS_CC;

bool GameScene::init()
{
    if (!Scene::init()) return false;

    scenemap = SceneMap::create();
    this->addChild(scenemap);

    SceneMapData = new class SceneMapData();

    return true;
}