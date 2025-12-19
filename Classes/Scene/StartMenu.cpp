#include "StartMenu.h"
#include "Loading.h"
#include "ui/CocosGUI.h"
#include "Res/strings.h"
#include "AudioEngine.h"
#include "Prison.h"  
#include "D:\Code\Cocos2dx\DeadCells\Classes\People\PlayerLayer.h"
#include "D:\Code\Cocos2dx\DeadCells\Classes\People\MonsterLayer.h"
USING_NS_CC;

cocos2d::Scene* StartMenu::createSceneWithPhysics()
{
	cocos2d::Scene* physicsScene = cocos2d::Scene::createWithPhysics();

	physicsScene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0, -980.0f));
	physicsScene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	StartMenu* pRet = new (std::nothrow) StartMenu();

	if (pRet && pRet->init())
	{
		pRet->autorelease();
		physicsScene->addChild(pRet);
		return physicsScene;
	}

	CC_SAFE_DELETE(pRet);
	return nullptr;
}
bool StartMenu::init()
{
	if (!Layer::init()) return false;
	
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto backGround = Sprite::create("Graph/StartMenu/StartMenuBackGround.jpg");
	this->addChild(backGround, -1);


	auto playerLayer = PlayerLayer::create();
	playerLayer->setPosition(Vec2(visibleSize.width / 5, 25 + 50));
	this->addChild(playerLayer, 1);

	auto grenadierLayer = MonsterLayer::create(MonsterCategory::Grenadier);
	grenadierLayer->setPosition(Vec2(visibleSize.width / 7, 25 + 50));
	this->addChild(grenadierLayer, 1);

	return true;
}

void StartMenu::onEnter()
{
	Layer::onEnter();

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto world = this->getScene()->getPhysicsWorld();

	if (world)
	{
		cocos2d::Size groundSize(visibleSize.width * 2, 50);
		auto groundBody = cocos2d::PhysicsBody::createEdgeBox(
			groundSize,
			cocos2d::PhysicsMaterial(0.1f, 0.0f, 0.0f));

		groundBody->setDynamic(false);

		auto groundNode = cocos2d::Node::create();
		groundNode->setPhysicsBody(groundBody);

		groundNode->setPosition(
			visibleSize.width / 2, // ¾ÓÖÐ
			groundSize.height / 2);
		groundBody->setCategoryBitmask(GROUND);
		groundBody->setCollisionBitmask(PLAYER_BODY | ENEMY_BODY);
		groundBody->setContactTestBitmask(PLAYER_BODY | ENEMY_BODY);
		this->addChild(groundNode);
		CCLOG("Physics Ground Added in onEnter.");

		// 
	}
}