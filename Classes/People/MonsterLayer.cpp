#include "MonsterLayer.h"
#include "PlayerLayer.h"

USING_NS_CC;

/**
 * 静态工厂函数
 * @param category 怪物种类（僵尸、射手或掷弹兵）
 * @param pos 怪物初始生成的坐标
 */
MonsterLayer* MonsterLayer::create(MonsterCategory category, cocos2d::Vec2 pos)
{
    MonsterLayer* pRet = new(std::nothrow) MonsterLayer();
    if (pRet && pRet->init(category, pos))
    {
        pRet->autorelease(); // 进入 Cocos2d-x 自动内存管理池
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

/**
 * 初始化怪物层
 */
bool MonsterLayer::init(MonsterCategory category, cocos2d::Vec2 pos)
{
    if (!Layer::init())
        return false;

    this->setName("MonsterLayer");

    // 调用 Monster 类的工厂方法创建具体的怪物对象
    _monster = Monster::create(category);
    if (!_monster) return false;

    // 设置位置并将其作为子节点添加，使其显示在屏幕上
    _monster->setPosition(pos);
    this->addChild(_monster);

    // 开启 update 回调，虽然这里开启了，但实际 AI 逻辑在自定义的 update(dt, playerPos) 中执行
    this->scheduleUpdate();
    return true;
}

/**
 * 获取内部封装的怪物实例指针
 */
Monster* MonsterLayer::getMonster()
{
    return _monster;
}

/**
 * 自定义更新函数：驱动 AI 逻辑
 * @param dt 帧间隔时间
 * @param playerPos 玩家当前的世界坐标（AI 决策的关键输入）
 */
void MonsterLayer::update(float dt, cocos2d::Vec2 playerPos)
{
    if (!_monster)
        return;

    // 将玩家位置传递给怪物的 AI 系统
    // 怪物会根据此位置决定是巡逻、追击还是发起攻击
    _monster->ai(dt, playerPos);
}