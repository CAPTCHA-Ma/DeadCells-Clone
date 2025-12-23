#ifndef __ARROW_H__
#define __ARROW_H__
#include "FlyingObject.h"

class Arrow : public FlyingObject
{
public:
	static Arrow* create(bool fromPlayer);
    bool init(bool fromPlayer);
	virtual void run(cocos2d::Vec2 targetPos) override;
private:
	MoveDirection _direction;
	float _speed = 400.0f;
};

#endif // __ARROW_H__