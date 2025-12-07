#pragma once
#include "cocos2d.h"
#include "Scene/GameScene.h"
#include <string>
#include <functional>

class Loading : public cocos2d::Scene
{
public:

	virtual bool init() override;

	CREATE_FUNC(Loading);

};