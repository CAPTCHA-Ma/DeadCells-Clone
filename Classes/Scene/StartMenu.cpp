#include "StartMenu.h"
#include "Loading.h"
#include "ui/CocosGUI.h"
#include "Res/strings.h"
#include "AudioEngine.h"
#include "Prison.h"
#include "People/people.h"
#include "People/PlayerLayer.h"
#include "People/MonsterLayer.h"
USING_NS_CC;

bool StartMenu::init()
{
	//if (!Scene::init()) return false;
	if (!Scene::initWithPhysics()) return false;
	this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	this->getPhysicsWorld()->setGravity(Vec2(0, -980.0f));


	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto groundNode = Node::create();
	auto groundBody = PhysicsBody::createBox(Size(visibleSize.width, 20.0f),
		PhysicsMaterial(0.1f, 1.0f, 0.0f));
	groundBody->setDynamic(false);
	groundBody->setCategoryBitmask(GROUND);
	groundBody->setCollisionBitmask(0xFFFFFFFF); // 与所有东西碰撞
	groundBody->setContactTestBitmask(0xFFFFFFFF);
	groundNode->setPhysicsBody(groundBody);
	groundNode->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + 10));
	this->addChild(groundNode);
	
	auto _playerLayer = PlayerLayer::create();
	this->addChild(_playerLayer, 1);
	auto _monsterLayer1 = MonsterLayer::create(MonsterCategory::Grenadier,Vec2(100,200)); 
	this->addChild(_monsterLayer1, 1);
	auto _monsterLayer2 = MonsterLayer::create(MonsterCategory::Zombie, Vec2(500,500));
	this->addChild(_monsterLayer2, 1);







	// 背景图
	auto backGround = Sprite::create("Graph/StartMenu/StartMenuBackGround.jpg");
	backGround->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	Size bgSize = backGround->getContentSize();

	float scaleX = visibleSize.width / bgSize.width;
	float scaleY = visibleSize.height / bgSize.height;

	backGround->setScale(std::max(scaleX, scaleY));

	this->addChild(backGround, -1);

	// 右下角logo
	auto Logo = Sprite::create("Graph/StartMenu/Logo.png");
	Logo->setAnchorPoint(Vec2(1, 0));
	Logo->setPosition(Vec2(visibleSize.width + origin.x, origin.y));
	Logo->setScale(std::max(scaleX, scaleY));

	this->addChild(Logo, 0);

	// 开始按钮和退出按钮
	auto StartButton = ui::Button::create();
	StartButton->setTitleText(GetText("start_game_text"));
	StartButton->setTitleFontName("fonts/fusion-pixel.ttf");
	StartButton->setTitleFontSize(40);
	StartButton->setAnchorPoint(Vec2(1, 0));
	StartButton->setPosition(Vec2(visibleSize.width + origin.x - 10, origin.y + visibleSize.height / 4 + 15));
	auto Startlabel = dynamic_cast<cocos2d::Label*>(StartButton->getTitleRenderer());
	Startlabel->enableShadow();
	Startlabel->enableGlow(Color4B::WHITE);

	/*StartButton->addClickEventListener([](Ref* sender) {

		auto LoadingScene = Loading::create();
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, LoadingScene));

		std::thread PrisonLoadingThread([]() {

			Prison* prisonScene = new class Prison;
			if (prisonScene->InitPrisonData()) {

				cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {

					prisonScene->RenderPrisonScene();

					});

			}

			});

		PrisonLoadingThread.detach();

		});*/

	StartButton->addClickEventListener([](Ref* sender) {

		Prison* prisonScene = new class Prison;

		prisonScene->SetupVisualScene();

		std::thread PrisonLoadingThread([=]() {

			prisonScene->InitPrisonData();

			});

		PrisonLoadingThread.detach();

		});

	this->addChild(StartButton);

	auto ExitButton = ui::Button::create();
	ExitButton->setTitleText(GetText("exit_text"));
	ExitButton->setTitleFontName("fonts/fusion-pixel.ttf");
	ExitButton->setTitleFontSize(40);
	ExitButton->setAnchorPoint(Vec2(1, 0));
	ExitButton->setPosition(Vec2(visibleSize.width + origin.x - 10, origin.y + visibleSize.height / 4 - 40));
	auto Exitlabel = dynamic_cast<cocos2d::Label*>(ExitButton->getTitleRenderer());
	Exitlabel->enableShadow();
	Exitlabel->enableGlow(Color4B::WHITE);

	ExitButton->addClickEventListener([](Ref* sender) {
		// 退出游戏
		Director::getInstance()->end();
		});

	this->addChild(ExitButton);

	return true;

}