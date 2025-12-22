#include "Bomb.h"
USING_NS_CC;
const float targetWidth = 50.0f;
const float targetHeight = 50.0f;
bool Bomb::init()
{
	if (!Sprite::initWithFile("Graph/FlyingObject/oilBomb-=-0-=-.png"))
		return false;
	this->_xSpeed = 200.0f;

	auto hurtbody = PhysicsBody::createBox(Size(targetWidth, targetHeight), PhysicsMaterial(0, 0, 0));
	hurtbody->setDynamic(true);
	hurtbody->setRotationEnable(false);
	hurtbody->setGravityEnable(true);
	this->setPhysicsBody(hurtbody);

	this->getPhysicsBody()->setCategoryBitmask(ENEMY_BOMB);
	this->getPhysicsBody()->setCollisionBitmask(0);
	this->getPhysicsBody()->setContactTestBitmask(PLAYER_HURT);
	return true;
}
void Bomb::run(cocos2d::Vec2 targetPos)
{
    auto body = this->getPhysicsBody();
    if (!body) 
        return;
    float g = abs(this->getScene()->getPhysicsWorld()->getGravity().y);
    if (g == 0) 
        g = 980.0f;
    Vec2 currentPos = this->getPosition();
    float dx = targetPos.x - currentPos.x;
    float dy = targetPos.y - currentPos.y;
    float h = 150.0f;
    if (dy > 0) h = dy + 50.0f;
    float vy = sqrt(2 * g * h);
    float discriminant = vy * vy - 2 * g * dy;
    float t = 0.0f;
    if (discriminant >= 0) 
        t = (vy + sqrt(discriminant)) / g;
    else 
        t = 1.0f;
    float vx = dx / t;
    body->setVelocity(Vec2(vx, vy));
    this->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
}
