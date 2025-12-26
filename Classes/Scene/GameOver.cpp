#include "GameOver.h"

cocos2d::Scene* GameOver::createScene()
{

    return GameOver::create();

}

bool GameOver::init() 
{

    if (!Scene::init()) return false;

    auto label = cocos2d::Label::createWithSystemFont("Level Clear!", "Arial", 40);
    label->setPosition(cocos2d::Director::getInstance()->getVisibleSize() / 2);
    this->addChild(label);

    return true;

}