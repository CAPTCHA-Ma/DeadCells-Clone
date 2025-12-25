#ifndef __BOMB_H__
#define __BOMB_H__
#include "FlyingObject.h"

class Bomb : public FlyingObject
{
public:
	CREATE_FUNC(Bomb);
	bool init();
	virtual void run(cocos2d::Vec2 targetPos) override;
	void explode();
	bool isExploded() const { return _isExploded; }
private:
	bool _isExploded = false; // ³õÊ¼»¯Îª false
	MoveDirection _direction;
	float _xSpeed = 400.0f;
};

#endif // __ARROW_H__