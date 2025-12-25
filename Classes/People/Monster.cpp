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
    _monsterAttributes.health -= attackPower * (100 - _monsterAttributes.defense)/100;

    if (_monsterAttributes.health <= 0)
    {
        dead();
    }
}
// 在 Monster.cpp (基类) 中修改
void Monster::dead()
{
    if (_isDead) return;
    _isDead = true;
    this->stopAllActions();
    if (_sprite) 
        _sprite->stopAllActions();


    if (_body) 
    {
        _body->setVelocity(Vec2::ZERO);
        _body->setContactTestBitmask(0);
        _body->setCollisionBitmask(GROUND); 
    }
    auto parentLayer = dynamic_cast<Layer*>(this->getParent());
    if (parentLayer) 
    {
        parentLayer->unscheduleUpdate();
    }

    onDead();
}

