#include "Loading.h"
#include "Res/strings.h"
#include <thread>

USING_NS_CC;

bool Loading::init()
{

	if (!Scene::init()) return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto loadingSprite = Sprite::create("Graph/Loading/Loading_0.png");
	auto Animation = Animation::create();

	for (int i = 0; i <= 19; i++)
	{

		std::string frameName = StringUtils::format("Graph/Loading/Loading_%d.png", i);
		Animation->addSpriteFrameWithFile(frameName);

	}

	Animation->setDelayPerUnit(0.1f);
	Animation->setRestoreOriginalFrame(true);

	auto animate = Animate::create(Animation);
	auto repeatAction = RepeatForever::create(animate);

	loadingSprite->runAction(repeatAction);

	loadingSprite->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height / 4 - 25));
	this->addChild(loadingSprite);

	auto loadingLabel = Label::createWithTTF(GetText("loading_text"), "fonts/fusion-pixel.ttf", 25);
	loadingLabel->setPosition(Vec2(origin.x + visibleSize.width - 95, origin.y + visibleSize.height / 4 - 75));
	this->addChild(loadingLabel);

	return true;

}