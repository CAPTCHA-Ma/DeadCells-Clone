#include "Arrow.h"
USING_NS_CC;
const float targetWidth = 50.0f;
const float targetHeight = 25.0f;
Arrow* Arrow::create(bool fromPlayer,float attackPower)
{
	Arrow* pRet = new(std::nothrow) Arrow(); 
		if (pRet && pRet->init(fromPlayer,attackPower)) 
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
bool Arrow::init(bool fromPlayer, float attackPower)
{
    if (!Sprite::initWithFile("Graph/FlyingObject/heavyArrow-=-0-=-.png"))
        return false;

    this->_speed = 400.0f; 
    this->_hasHit = false; 

    auto hurtbody = PhysicsBody::createBox(Size(targetWidth * 0.8f, targetHeight), PhysicsMaterial(0.1f, 0.0f, 0.0f));

	hurtbody->setDynamic(true);
	hurtbody->setGravityEnable(false);

    if (fromPlayer)
    {
        hurtbody->setCategoryBitmask(PLAYER_ARROW);
        hurtbody->setCollisionBitmask(PhysicsCategory::GROUND);
        hurtbody->setContactTestBitmask(ENEMY_BODY | GROUND);
    }
    else
    {
        hurtbody->setCategoryBitmask(ENEMY_ARROW);
        hurtbody->setCollisionBitmask(PhysicsCategory::GROUND);
        hurtbody->setContactTestBitmask(PLAYER_BODY | GROUND);
    }
	this->setAttackPower(attackPower);
    this->setPhysicsBody(hurtbody);
    this->runAction(Sequence::create(DelayTime::create(8.0f), RemoveSelf::create(), nullptr));

    return true;
}
void Arrow::run(cocos2d::Vec2 targetPos)

{

	float speed = _speed;
	auto body = this->getPhysicsBody();
	if (body)
		body->setVelocity(targetPos * speed);
	if (targetPos.x < 0)
		_direction = MoveDirection::LEFT;
	else
		_direction = MoveDirection::RIGHT;
	this->setFlippedX(_direction == MoveDirection::LEFT);
}
void Arrow::hit() 
{
	if (_hasHit) return;
	_hasHit = true;

	this->getPhysicsBody()->setContactTestBitmask(0);
	this->getPhysicsBody()->setVelocity(Vec2::ZERO);

	this->runAction(Sequence::create(
		FadeOut::create(0.2f),
		RemoveSelf::create(),
		nullptr
	));
}
