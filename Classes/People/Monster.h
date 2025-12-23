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
    virtual void onDead() = 0;
    void struck(float attackPower);
	void dead();
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) = 0;
    static Monster* create(MonsterCategory category);
    MonsterCategory getMonsterType();
protected:
    CC_SYNTHESIZE(BasicAttributes, _monsterAttributes, MonsterAttributes);
    MonsterCategory _type;
    bool _isDead = false;
    bool _isReadyToRemove = false; // 新增：标记动画播放完毕，可以从容器删除
    cocos2d::Sprite* _sprite = nullptr;
    cocos2d::PhysicsBody* _body = nullptr;

    cocos2d::Node* _attackNode = nullptr;
    cocos2d::Node* _hurtNode = nullptr;
	virtual void createHurtBox() = 0;
    virtual void createAttackBox()=0;
    void removeAttackBox();
    
public:
    bool isDead() const { return _isDead; }
    bool isReadyToRemove() const { return _isReadyToRemove; }
};


#endif // __MONSTER_H__