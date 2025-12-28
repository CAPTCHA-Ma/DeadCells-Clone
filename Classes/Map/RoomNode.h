#pragma once
#include "cocos2d.h"
#include "MapUnit.h"
#include "TileRenderer.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include "People/MonsterLayer.h"
#include "People/Monster.h"
#include "People/WeaponNode.h"
#include "People/Sword.h"
#include "People/Bow.h"
#include "People/Shield.h"

// 单元房间生成类
class RoomNode : public cocos2d::Node
{
public:

	static RoomNode* create(MapUnitData* data, cocos2d::Vector<MonsterLayer*>& _monsters);
	bool init(MapUnitData* data, cocos2d::Vector<MonsterLayer*>& _monsters);

private:
	TileRenderer* _tileRenderer = nullptr;
};