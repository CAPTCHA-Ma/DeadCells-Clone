#include "Bomb.h"
USING_NS_CC;
const float targetWidth = 30.0f;
const float targetHeight = 30.0f;
bool Bomb::init()
{
	if (!Sprite::initWithFile("Graph/FlyingObject/oilBomb-=-0-=-.png"))
		return false;
	this->_xSpeed = 200.0f;

	auto body = PhysicsBody::createBox(Size(targetWidth, targetHeight), PhysicsMaterial(0, 0, 0));
    body->setDynamic(true);

    body->setCategoryBitmask(PhysicsCategory::ENEMY_BOMB);
    body->setCollisionBitmask(PhysicsCategory::GROUND); 
    body->setContactTestBitmask(PhysicsCategory::PLAYER_BODY | PhysicsCategory::GROUND);

    this->setPhysicsBody(body);
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
    float h = 20.0f;
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
void Bomb::explode()
{
    if (_isExploded) 
        return;
    _isExploded = true;
    auto body = this->getPhysicsBody();
    if (body) 
    {
        body->setVelocity(Vec2::ZERO);
        body->setContactTestBitmask(0); 
        body->setGravityEnable(false); 
    }
    this->stopAllActions(); 
    auto scaleUp = ScaleTo::create(0.1f, 2.0f);
    auto tint = TintTo::create(0.1f, 255, 100, 100); 
    auto fade = FadeOut::create(0.15f);
    auto remove = RemoveSelf::create();
    auto explodeSequence = Sequence::create(
        Spawn::create(scaleUp, tint, nullptr),
        fade,
        remove,
        nullptr
    );
    this->runAction(explodeSequence);
}