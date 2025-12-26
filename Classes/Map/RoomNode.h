#pragma once
#include "cocos2d.h"
#include "MapUnit.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include "People/MonsterLayer.h"
#include "People/Monster.h"

class RoomNode : public cocos2d::Node
{
public:

	static RoomNode* create(MapUnitData* data, cocos2d::Vector<MonsterLayer*>& _monsters);
	bool init(MapUnitData* data, cocos2d::Vector<MonsterLayer*>& _monsters);

};