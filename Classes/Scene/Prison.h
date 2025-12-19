#pragma once
#include "cocos2d.h"
#include "Map/SceneMap.h"
#include "People/Player.h"
#define CORR_WIDTH 10
#define PUSH_FORCE 10.0f
#define PULL_FORCE 5.0f
#define MAX_TEMP 10000
#define BLOCK_WEIGHT 1000000
#define SURROUND_WEIGHT 10
USING_NS_CC;

class Prison
{
public:

	void SetupVisualScene();
	bool InitPrisonData();
	bool RenderPrisonScene();
	void GenRoomPos();

	SceneMap* PrisonMap = nullptr;
	SceneMapData* PrisonMapData = nullptr;
	Scene* PrisonScene = nullptr;
	MapDataManager* mDM = nullptr;
	cocos2d::DrawNode* _debugDrawNode = nullptr;
	std::vector<std::vector<int>> MapArray;
	void Draw();

};
