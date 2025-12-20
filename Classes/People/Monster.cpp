// Monster.cpp
#include "Monster.h"
#include "Grenadier.h"
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
        default:
            return nullptr;
    }
}                          