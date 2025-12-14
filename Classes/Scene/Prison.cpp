#include "Prison.h"
USING_NS_CC;

bool Prison::InitPrisonData()
{

	Sleep(2000);

	PrisonMapData = new class SceneMapData();

	return true;

}	

bool Prison::RenderPrisonScene()
{

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto prisonMap = TMXTiledMap::create("room/Prison/Pr2.tmx");
	prisonMap->setAnchorPoint(Vec2(0, origin.y / 2));
	prisonMap->setPosition(Vec2(origin.x, origin.y));

	PrisonScene = Scene::create();
	PrisonScene->addChild(prisonMap, 0);
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, PrisonScene));

	return true;

}