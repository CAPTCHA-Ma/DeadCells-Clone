#include "MonsterLayer.h"

USING_NS_CC;

MonsterLayer* MonsterLayer::create(MonsterCategory category, cocos2d::Vec2 pos)
{

    MonsterLayer* pRet = new(std::nothrow) MonsterLayer();
    if (pRet && pRet->init(category,pos))
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
	_monster = Monster::create(category);
    if (_monster)
    {
        // 2. 关键点：必须添加到当前层才能显示！
        this->addChild(_monster);

        // 3. 建议：设置一个初始坐标，否则默认在 (0,0) 可能在屏幕边缘
        _monster->setPosition(pos);
    }
    this->scheduleUpdate(); 
    return true;
}
void MonsterLayer::update(float dt)
{
    // 假设你的 MonsterLayer 里管理着一个怪物实例 _monster
    if (!_monster) return;

    // 1. 获取场景和 PlayerLayer
    auto scene = this->getScene(); // getScene() 比 getParent() 更稳定
    if (!scene) return;

    auto playerLayer = scene->getChildByName("PlayerLayer");
    if (!playerLayer) return;

    // 2. 获取 Player 对象
    // 建议在 PlayerLayer 类里写一个 getPlayer() 方法，而不是通过名字找
    auto player = playerLayer->getChildByName("Player");
    if (!player) return;

    // 3. 获取玩家的世界坐标（关键修改）
    // 使用父节点 convertToWorldSpace 玩家的 Position，得到的结果最准确
    Vec2 playerWorldPos = player->getParent()->convertToWorldSpace(player->getPosition());

    // 4. 更新 AI
    _monster->ai(dt, playerWorldPos);

}