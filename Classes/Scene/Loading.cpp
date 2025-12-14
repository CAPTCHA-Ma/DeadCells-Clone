#include "Loading.h"
#include "Res/strings.h"
#include "Resprite/Resprite.h"

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

	loadingSprite->setPosition(Vec2(origin.x + visibleSize.width - 200, origin.y + visibleSize.height / 4 - 50));
	loadingSprite->setScale(2.0f);
	this->addChild(loadingSprite);

	auto loadingLabel = Label::createWithTTF(GetText("loading_text"), "fonts/fusion-pixel.ttf", 50);
	loadingLabel->setPosition(Vec2(origin.x + visibleSize.width - 190, origin.y + visibleSize.height / 4 - 150));
	this->addChild(loadingLabel);

	auto characterSprite = Resprite::create("Graph/Loading/Character.png", "Graph/Loading/CharacterPalette.png", "Graph/Loading/CharacterNormal.png");
	characterSprite->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	characterSprite->setScale(10.0f);
	this->addChild(characterSprite);

	return true;

}