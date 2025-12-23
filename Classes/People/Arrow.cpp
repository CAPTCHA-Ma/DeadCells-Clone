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

    this->_speed = 400.0f; // 适当提升速度增加打击感
    this->_hasHit = false; // 建议在头文件定义此变量，防止一次碰撞多次逻辑

    // 优化碰撞盒：Size 建议略小于图片，避免箭尾擦边触发碰撞
    auto hurtbody = PhysicsBody::createBox(Size(targetWidth * 0.8f, targetHeight), PhysicsMaterial(0.1f, 0.0f, 0.0f));

    hurtbody->setDynamic(true);
    hurtbody->setRotationEnable(false);
    hurtbody->setGravityEnable(false);


    if (fromPlayer)
    {
        hurtbody->setCategoryBitmask(PLAYER_ARROW);
        // --- 核心修复：必须包含 GROUND，否则无法被地面挡住 ---
        hurtbody->setCollisionBitmask(PhysicsCategory::GROUND);
        // 监听敌人和地面
        hurtbody->setContactTestBitmask(ENEMY_HURT | GROUND);
    }
    else
    {
        hurtbody->setCategoryBitmask(ENEMY_ARROW);
        // 敌人的箭也需要被地面挡住
        hurtbody->setCollisionBitmask(PhysicsCategory::GROUND);
        // 监听玩家和地面
        hurtbody->setContactTestBitmask(PLAYER_HURT | GROUND);
    }

    this->setPhysicsBody(hurtbody);

    // 生命周期管理
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
