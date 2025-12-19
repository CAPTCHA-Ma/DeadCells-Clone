#ifndef __MONSTER_LAYER_H__
#define __MONSTER_LAYER_H__
#include "Monster.h"
class MonsterLayer : public cocos2d::Layer
{
public:

	static MonsterLayer* create(MonsterCategory category);
	void update(float dt);
private:
	bool init(MonsterCategory category);
	Monster* _monster;
	float _aiTickTimer;
};
#endif // __MONSTER_LAYER_H__