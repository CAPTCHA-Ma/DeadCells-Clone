#ifndef __FLYINGOBJECT_H__
#define __FLYINGOBJECT_H__
#include "cocos2d.h"
#include "People.h"
enum class FlyType
{
	Arrow,
	Bomb
};
class FlyingObject : public cocos2d::Sprite
{
public:
	virtual void run(cocos2d::Vec2 targetPos) = 0;
	static FlyingObject* create(FlyType type, bool fromPlayer, float atkPower);
protected:
	cocos2d::Vec2 _velocity; // ËÙ¶È
	CC_SYNTHESIZE(float, _attackPower, AttackPower);
};
#endif // __FLYINGOBJECT_H__