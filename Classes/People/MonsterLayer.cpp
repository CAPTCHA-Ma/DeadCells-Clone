#include "MonsterLayer.h"
USING_NS_CC;
MonsterLayer* MonsterLayer::create(MonsterCategory category)
{

    MonsterLayer* pRet = new(std::nothrow) MonsterLayer();
    if (pRet && pRet->init(category))
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

bool MonsterLayer::init(MonsterCategory category)
{
    if (!Layer::init())
        return false;
	_monster = Monster::create(category);
    if (_monster) {
        // 2. 关键点：必须添加到当前层才能显示！
        this->addChild(_monster);

        // 3. 建议：设置一个初始坐标，否则默认在 (0,0) 可能在屏幕边缘
        _monster->setPosition(Vec2(200, 200));
    }
    this->scheduleUpdate(); 
    return true;
}
void MonsterLayer::update(float dt)
{
    if (!_monster || _monster->_dead) return;

    // 1. 获取玩家对象
    auto scene = this->getParent();
    auto playerLayer = scene->getChildByName("PlayerLayer");
    if (!playerLayer) return;

    // 2. 获取玩家的世界坐标
    Vec2 playerWorldPos = playerLayer->convertToWorldSpace(Vec2::ZERO);

    // 3. 直接调用 Monster 自己的 AI 函数
    // 传 dt 是为了让怪物自己控制思考频率，传坐标是为了让它自己算距离
    _monster->ai(dt, playerWorldPos);
}