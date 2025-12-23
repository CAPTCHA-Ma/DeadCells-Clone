// Monster.cpp
#include "Grenadier.h"
#include "Zombie.h"
USING_NS_CC;
void Monster::removeAttackBox()
{
    if (_attackNode)
    {
        _attackNode->removeFromParent();
        _attackNode = nullptr;
    }
}
Monster* Monster::create(MonsterCategory category)
{
    switch (category)
    {
        case MonsterCategory::Grenadier:
            return Grenadier::create();
        case MonsterCategory::Zombie:
            return Zombie::create();
        default:
            return nullptr;
    }
}
MonsterCategory Monster::getMonsterType()
{
    return this->_type;
}
void Monster::struck(float attackPower)
{
    _monsterAttributes.health -= attackPower * (1 - _monsterAttributes.defense / 100);

    if (_monsterAttributes.health <= 0)
    {
        dead();
    }
}
void Monster::dead()
{
    if (_isDead)
        return;

    _isDead = true;

    stopAllActions();

    if (_body)
    {
        _body->setVelocity(Vec2::ZERO);
        _body->setGravityEnable(false);
        _body->setContactTestBitmask(0);
        _body->setCollisionBitmask(0);
    }

    onDead(); 
}

