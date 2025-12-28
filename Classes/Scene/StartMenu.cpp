#include "StartMenu.h"
#include "Prison.h"
#include "ui/CocosGUI.h"
#include "Res/strings.h"
#include "AudioEngine.h"

USING_NS_CC;

bool StartMenu::init()
{
    if (!Scene::init()) return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // --- 背景图 ---
    auto backGround = Sprite::create("Graph/StartMenu/StartMenuBackGround.jpg");
    backGround->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    Size bgSize = backGround->getContentSize();
    float scaleX = visibleSize.width / bgSize.width;
    float scaleY = visibleSize.height / bgSize.height;
    backGround->setScale(std::max(scaleX, scaleY));
    this->addChild(backGround, -1);

    // --- 右下角 Logo ---
    auto Logo = Sprite::create("Graph/StartMenu/Logo.png");
    Logo->setAnchorPoint(Vec2(1, 0));
    Logo->setPosition(Vec2(visibleSize.width + origin.x, origin.y));
    Logo->setScale(std::max(scaleX, scaleY));
    this->addChild(Logo, 0);

    // 定义按钮的 X 坐标 (右对齐，稍微往左一点)
    float buttonX = visibleSize.width + origin.x - 10;
    // 定义基准高度
    float baseY = origin.y + visibleSize.height / 4;

    // --- 1. 开始按钮 ---
    auto StartButton = ui::Button::create();
    StartButton->setTitleText(GetText("start_game_text"));
    StartButton->setTitleFontName("fonts/fusion-pixel.ttf");
    StartButton->setTitleFontSize(40);
    StartButton->setAnchorPoint(Vec2(1, 0));
    StartButton->setPosition(Vec2(buttonX, baseY + 60));

    auto Startlabel = dynamic_cast<cocos2d::Label*>(StartButton->getTitleRenderer());
    Startlabel->enableShadow();
    Startlabel->enableGlow(Color4B::WHITE);

    StartButton->addClickEventListener([](Ref* sender) {
        auto prisonGen = new PrisonMapGen();
        auto prisonScene = GameScene::createWithGenerator(prisonGen);
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, prisonScene));
        });
    this->addChild(StartButton);

	// --- 2. 帮助按钮 ---
    auto HelpButton = ui::Button::create();
    HelpButton->setTitleText(GetText("help_menu"));
    HelpButton->setTitleFontName("fonts/fusion-pixel.ttf");

    HelpButton->setTitleFontSize(40);
    HelpButton->setAnchorPoint(Vec2(1, 0));
    HelpButton->setPosition(Vec2(buttonX, baseY));

    HelpButton->setContentSize(Size(200, 50));

    auto Helplabel = dynamic_cast<cocos2d::Label*>(HelpButton->getTitleRenderer());
    if (Helplabel) { 
        Helplabel->enableShadow();
        Helplabel->enableGlow(Color4B::WHITE);
    }

    HelpButton->addClickEventListener([=](Ref* sender) {
        auto helpLayer = LayerColor::create(Color4B(0, 0, 0, 200));

        std::string instructions = GetText("game_instructions");

        auto infoLabel = Label::createWithTTF(instructions, "fonts/fusion-pixel.ttf", 24);

        if (!infoLabel) {
            infoLabel = Label::createWithSystemFont(instructions, "Arial", 24);
        }

        infoLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2);
        helpLayer->addChild(infoLabel);

        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->setSwallowTouches(true);
        touchListener->onTouchBegan = [=](Touch* touch, Event* event) {
            helpLayer->removeFromParent();
            return true;
            };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, helpLayer);

        this->addChild(helpLayer, 100);
        });

    this->addChild(HelpButton);

    // --- 3. 退出按钮 (位置下移) ---
    auto ExitButton = ui::Button::create();
    ExitButton->setTitleText(GetText("exit_text"));
    ExitButton->setTitleFontName("fonts/fusion-pixel.ttf");
    ExitButton->setTitleFontSize(40);
    ExitButton->setAnchorPoint(Vec2(1, 0));
    ExitButton->setPosition(Vec2(buttonX, baseY - 60));

    auto Exitlabel = dynamic_cast<cocos2d::Label*>(ExitButton->getTitleRenderer());
    Exitlabel->enableShadow();
    Exitlabel->enableGlow(Color4B::WHITE);

    ExitButton->addClickEventListener([](Ref* sender) {
        Director::getInstance()->end();
        });
    this->addChild(ExitButton);

    return true;
}