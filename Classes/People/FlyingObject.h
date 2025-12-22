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
	static FlyingObject* create(FlyType type, bool fromPlayer);
protected:
	cocos2d::Vec2 _velocity; // ËÙ¶È
};
#endif // __FLYINGOBJECT_H__