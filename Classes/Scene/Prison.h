#pragma once
#include "cocos2d.h"
#include "Scene/GameScene.h"
#include "Map/MapGenerator.h"

#define CORR_WIDTH 10	// 预留走廊宽度
#define PUSH_FORCE 10.0f	// 推开重叠房间的力度
#define PULL_FORCE 5.0f		// 拉近关联房间的力度
#define MAX_POS_TEMP 1000	// 最大力导向迭代次数
#define MAX_TEMP 3		// 最大生成走廊次数
#define TURN_PUNISH 20		// 转向惩罚值
#define BLOCK_WEIGHT 1000000	// 障碍物权重

USING_NS_CC;

// 第一关监狱地图生成器
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
