#pragma once
#include "cocos2d.h"
#include "MapUnit.h"
#include <vector>

class SceneMapData
{
public:

	std::vector<MapUnitData*> SceneMapDataUnit;

};

class SceneMap : public cocos2d::Node
{
public:

	virtual bool init() override;

	CREATE_FUNC(SceneMap);

	cocos2d::Vector<MapUnit*> SceneMapUnit;

};