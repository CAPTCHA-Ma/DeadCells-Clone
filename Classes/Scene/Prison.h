#pragma once
#include "cocos2d.h"
#include "Scene/GameScene.h"
#include "Map/MapGenerator.h"

#define CORR_WIDTH 10
#define PUSH_FORCE 10.0f
#define PULL_FORCE 5.0f
#define MAX_TEMP 5
#define TURN_PUNISH 20
#define BLOCK_WEIGHT 1000000
#define SURROUND_WEIGHT 10
USING_NS_CC;

class PrisonMapGen : public MapGenerator
{
public:

	virtual void Generate() override;
	virtual void GenRoomPos() override;
	virtual bool FindPath(std::vector<std::vector<int>>& mapArray,
		std::vector<std::vector<AStarNode>>& AStarGraph,
		Vec2 startPos,
		Vec2 endPos,
		std::vector<Vec2>& path) override;

};
