#pragma once
#include "cocos2d.h"
#include "MapUnit.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

class RoomNode : public cocos2d::Node
{
public:

	static RoomNode* create(MapUnitData* data);
	bool init(MapUnitData* data);

};