#ifndef __MONSTER_LAYER_H__
#define __MONSTER_LAYER_H__
#include "Monster.h"

class MonsterLayer : public cocos2d::Layer
{
public:

	static MonsterLayer* create(MonsterCategory category, cocos2d::Vec2 pos);
	void update(float dt,cocos2d::Vec2 playerPos);
	void struck(float attackPower);
	Monster* getMonster();

private:
	bool init(MonsterCategory category, cocos2d::Vec2 pos);
	Monster* _monster;
	float _aiTickTimer;
};
#endif // __MONSTER_LAYER_H__