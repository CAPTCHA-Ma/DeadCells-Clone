#include "GameOver.h"
#include "Map/MapUnit.h"

USING_NS_CC;

// 创建游戏结束场景
cocos2d::Scene* GameOver::createScene()
{

    return GameOver::create();

}

// 初始化游戏结束场景
bool GameOver::init() 
{

    if (!Scene::init()) return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto RestartButton = ui::Button::create();
	RestartButton->setTitleText(GetText("restart_text"));
	RestartButton->setTitleFontName("fonts/fusion-pixel.ttf");
	RestartButton->setTitleFontSize(40);
	RestartButton->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + visibleSize.height / 4 + 15));
	auto Restartlabel = dynamic_cast<cocos2d::Label*>(RestartButton->getTitleRenderer());
	Restartlabel->enableShadow();
	Restartlabel->enableGlow(Color4B::WHITE);

	RestartButton->addClickEventListener([](Ref* sender)
		{

			auto prisonGen = new PrisonMapGen();
			auto scene = GameScene::createWithGenerator(prisonGen);
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));

		});

	this->addChild(RestartButton);

	auto ExitButton = ui::Button::create();
	ExitButton->setTitleText(GetText("exit_text"));
	ExitButton->setTitleFontName("fonts/fusion-pixel.ttf");
	ExitButton->setTitleFontSize(40);
	ExitButton->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + visibleSize.height / 4 - 40));
	auto Exitlabel = dynamic_cast<cocos2d::Label*>(ExitButton->getTitleRenderer());
	Exitlabel->enableShadow();
	Exitlabel->enableGlow(Color4B::WHITE);

	ExitButton->addClickEventListener([](Ref* sender) {
		Director::getInstance()->end();
		});

	this->addChild(ExitButton);

	auto BackToMenuButton = ui::Button::create();
	BackToMenuButton->setTitleText(GetText("back_to_menu_text"));
	BackToMenuButton->setTitleFontName("fonts/fusion-pixel.ttf");
	BackToMenuButton->setTitleFontSize(40);
	BackToMenuButton->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + visibleSize.height / 4 - 95));
	auto BackToMenulabel = dynamic_cast<cocos2d::Label*>(BackToMenuButton->getTitleRenderer());
	BackToMenulabel->enableShadow();
	BackToMenulabel->enableGlow(Color4B::WHITE);

	BackToMenuButton->addClickEventListener([](Ref* sender) {
		auto StartMenuScene = StartMenu::create();
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, StartMenuScene));
		});

	this->addChild(BackToMenuButton);

    return true;

}