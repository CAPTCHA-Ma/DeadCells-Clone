#include "StartMenu.h"
#include "Loading.h"
#include "ui/CocosGUI.h"
#include "Res/strings.h"
#include "AudioEngine.h"
#include "Prison.h"
USING_NS_CC; 

bool StartMenu::init()
{

	if (!Scene::init()) return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

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

	StartButton->addClickEventListener([](Ref* sender) {

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