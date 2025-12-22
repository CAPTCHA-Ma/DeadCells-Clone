// Monster.h
#ifndef __MONSTER_H__
#define __MONSTER_H__
#include "cocos2d.h"
#include "People.h"
#include "Arrow.h"
#include "Bomb.h"
enum class MonsterCategory
{
    Grenadier,
    Zombie
};
class Monster : public cocos2d::Sprite
{
public:
    virtual bool init() = 0;

    virtual void idle()=0;     
	virtual void walk() = 0;   
	virtual void dead() = 0;
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) = 0;
    static Monster* create(MonsterCategory category);
protected:
    CC_SYNTHESIZE(BasicAttributes, _monsterAttributes, MonsterAttributes);
    bool _dead = false;
    cocos2d::Node* _attackNode = nullptr;
    cocos2d::Node* _hurtNode = nullptr;
	virtual void createHurtBox() = 0;
    virtual void createAttackBox()=0;
    void removeAttackBox();
};


#endif // __MONSTER_H__