#ifndef __BOMB_H__
#define __BOMB_H__
#include "FlyingObject.h"

class Bomb : public FlyingObject
{
public:
	CREATE_FUNC(Bomb);
	bool init();
	virtual void run(cocos2d::Vec2 targetPos) override;
private:
	MoveDirection _direction;
	float _xSpeed = 400.0f;
};

#endif // __ARROW_H__