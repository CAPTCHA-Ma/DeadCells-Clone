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