//#include "Arrow.h"
//USING_NS_CC;
//
//bool Arrow::init()
//{
//	if (!Sprite::initWithFile("Graph/Weapons/arrow.png"))
//		return false;
//	auto body = PhysicsBody::createBox(this->getContentSize(),
//		PhysicsMaterial(0.1f, 0.0f, 0.5f));
//	body->setDynamic(true);
//	body->setRotationEnable(false);
//	body->setGravityEnable(false); 
//	body->setVelocity(Vec2::ZERO);
//
//	this->setPhysicsBody(body);
//
//	this->getPhysicsBody()->setCategoryBitmask(0x04); // Àà±ðÑÚÂë4
//	this->getPhysicsBody()->setCollisionBitmask(0x02); // Åö×²ÑÚÂë2
//	this->scheduleUpdate();
//
//	return true;
//}
//void Arrow::setVelocity(const cocos2d::Vec2& velocity) 
//{ 
//	_velocity = velocity; 
//}