#ifndef __ARROW_H__
#define __ARROW_H__
#include "FlyingObject.h"

class Arrow : public FlyingObject
{
public:
	static Arrow* create(bool fromPlayer, float attackPower);
    bool init(bool fromPlayer, float attackPower);
	virtual void run(cocos2d::Vec2 targetPos) override;
	void hit();
	bool hasHit() { return _hasHit; };
private:
	MoveDirection _direction;
	float _speed = 400.0f;
	bool _hasHit;
};

#endif // __ARROW_H__