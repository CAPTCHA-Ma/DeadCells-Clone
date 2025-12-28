#ifndef __MONSTER_LAYER_H__
#define __MONSTER_LAYER_H__

#include "Monster.h"

/**
 * MonsterLayer 类：怪物容器层
 * 职责：负责特定怪物的创建、AI 驱动逻辑的分发以及与玩家交互的中转
 */
class MonsterLayer : public cocos2d::Layer
{
public:
    /**
     * 静态工厂方法
     * @param category 怪物种类（如：Zombie, DeadArcher, Grenadier）
     * @param pos 怪物生成的初始坐标
     */
    static MonsterLayer* create(MonsterCategory category, cocos2d::Vec2 pos);

    /**
     * 自定义更新函数
     * 由主场景（GameScene）每帧调用，将玩家位置传入，驱动怪物 AI 决策
     * @param dt 帧间隔时间
     * @param playerPos 玩家当前的世界坐标
     */
    void update(float dt, cocos2d::Vec2 playerPos);

    /**
     * 受击接口
     * 当该层内的怪物受到伤害时，由此接口统一处理数值下传
     * @param attackPower 玩家造成的攻击力
     */
    void struck(float attackPower);

    /**
     * 获取内部封装的怪物对象
     * 用于外部直接访问怪物的物理身体、血量或当前状态
     */
    Monster* getMonster();

private:
    /**
     * 初始化函数（设为私有，强制通过 create 调用）
     */
    bool init(MonsterCategory category, cocos2d::Vec2 pos);

    Monster* _monster;   // 指向具体的怪物实例（如 Zombie 实例）
    float _aiTickTimer;  // AI 逻辑计数器（通常用于控制非每帧执行的复杂决策）
};

#endif // __MONSTER_LAYER_H__