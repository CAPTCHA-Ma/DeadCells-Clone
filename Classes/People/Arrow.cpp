#include "Arrow.h"
USING_NS_CC;
const float targetWidth = 50.0f;
const float targetHeight = 25.0f;
Arrow* Arrow::create(bool fromPlayer)
{
	Arrow* pRet = new(std::nothrow) Arrow(); 
		if (pRet && pRet->init(fromPlayer)) 
		{ 
			pRet->autorelease(); 
			return pRet; 
		} 
		else 
		{ 
			delete pRet; 
			pRet = nullptr; 
			return nullptr; 
		}
}
bool Arrow::init(bool fromPlayer)
{
	if (!Sprite::initWithFile("Graph/FlyingObject/heavyArrow-=-0-=-.png"))
		return false;
	this->_speed = 200.0f;
	auto hurtbody = PhysicsBody::createBox(Size(targetWidth,targetHeight),PhysicsMaterial(0, 0, 0));
	hurtbody->setDynamic(true);
	hurtbody->setRotationEnable(false);
	hurtbody->setGravityEnable(false);
	this->setPhysicsBody(hurtbody);

	if (fromPlayer)
	{
		this->getPhysicsBody()->setCategoryBitmask(PLAYER_ARROW);
		this->getPhysicsBody()->setCollisionBitmask(0); 
		this->getPhysicsBody()->setContactTestBitmask(ENEMY_HURT); 
	}
	else
	{
		this->getPhysicsBody()->setCategoryBitmask(ENEMY_ARROW);
		this->getPhysicsBody()->setCollisionBitmask(0);
		this->getPhysicsBody()->setContactTestBitmask(PLAYER_HURT);
	}
	return true;
}
void Arrow::run(cocos2d::Vec2 targetPos)
{ 
	float speed = _speed;
	auto body = this->getPhysicsBody();
	if (body)
		body->setVelocity(targetPos * speed);
	if(targetPos.x < 0)
		_direction = MoveDirection::LEFT;
	else
		_direction = MoveDirection::RIGHT;
	this->setFlippedX(_direction == MoveDirection::LEFT);
}
