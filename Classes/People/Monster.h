// Monster.h
#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "cocos2d.h"

class Monster : public cocos2d::Sprite
{
public:
    // 1. 创建
    static Monster* createMonster(int monsterType);


    // 3. 怪物属性
    CC_SYNTHESIZE(int, _health, Health);     // 生命值
    CC_SYNTHESIZE(float, _speed, Speed);     // 移动速度

    // 4. 行为
    void moveTowards(const cocos2d::Vec2& targetPosition);


private:
    
    bool initWithType(int monsterType);
};

#endif // __MONSTER_H__