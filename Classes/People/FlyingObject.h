#ifndef __FLYINGOBJECT_H__
#define __FLYINGOBJECT_H__
#include "cocos2d.h"
class FlyingObject : public cocos2d::Sprite
{
public:
	virtual bool init()=0;
protected:
	cocos2d::Vec2 _velocity; // ËÙ¶È
}
#endif // __FLYINGOBJECT_H__