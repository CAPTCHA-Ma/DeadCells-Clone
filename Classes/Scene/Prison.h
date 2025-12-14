#pragma once
#include "cocos2d.h"
#include "Map/SceneMap.h"
USING_NS_CC;

class Prison
{
public:

	bool InitPrisonData();
	bool RenderPrisonScene();

	SceneMap* PrisonMap = nullptr;
	SceneMapData* PrisonMapData = nullptr;
	Scene* PrisonScene = nullptr;
	//Player* _player = nullptr;

};