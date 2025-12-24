#include "MonsterLayer.h"
#include "PlayerLayer.h"
USING_NS_CC;
MonsterLayer* MonsterLayer::create(MonsterCategory category, cocos2d::Vec2 pos)
{

    MonsterLayer* pRet = new(std::nothrow) MonsterLayer();
    if (pRet && pRet->init(category, pos))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool MonsterLayer::init(MonsterCategory category, cocos2d::Vec2 pos)
{
    if (!Layer::init())
        return false;

    this->setName("MonsterLayer");

    _monster = Monster::create(category);
    if (!_monster) return false;

    _monster->setPosition(pos);
    this->addChild(_monster);

    this->scheduleUpdate();
    return true;
}
Monster* MonsterLayer::getMonster()
{
    return _monster;
}
void MonsterLayer::update(float dt, cocos2d::Vec2 playerPos)
{
    if (!_monster)
        return;
    else
        _monster->ai(dt, playerPos);
}