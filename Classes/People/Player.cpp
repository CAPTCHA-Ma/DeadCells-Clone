// Player.cpp
#include "Player.h"

USING_NS_CC;
/***************************************************************************
  函数名称：init
  功    能：初始化主角
  输入参数：
  返 回 值：bool
  说    明：
***************************************************************************/
bool Player::init()
{
	std::string filename = "Player.png";
	if (!Sprite::initWithFile(filename))
		return false;
	// 设置初始属性
	this->setHealth(100);
	this->setAttack(20);
	this->setDefense(10);
	this->setMoveSpeed(100.0f); 
	_handNode = Node::create();
	_handNode->setPosition(Vec2(20, 10));//这个位置要改成手的位置
	this->addChild(_handNode,2);
	return true;
}


/***************************************************************************
  函数名称：createPlayer
  功    能：创建主角
  输入参数：
  返 回 值：Player*
  说    明：
***************************************************************************/
Player* Player::createPlayer()
{
	Player* player = new (std::nothrow)Player;
	if (player&&player->init())
	{
		player->autorelease();
		return player;
	}
	CC_SAFE_DELETE(player);
	return nullptr;
}
/***************************************************************************
  函数名称：equipWeapon
  功    能：装备武器
  输入参数：Weapon* weapon武器类别
  返 回 值：
  说    明：
***************************************************************************/
void Player::equipWeapon(Weapon* weapon)
{
	//已经装备了武器，卸下当前武器
	if (_equippedWeapon)
	{
		_equippedWeapon->removeFromParent();
		_equippedWeapon = nullptr;
	}
	//装备新武器
	_equippedWeapon = weapon;
	if (_equippedWeapon)
	{
		_handNode->addChild(_equippedWeapon);
		_equippedWeapon->setPosition(Vec2::ZERO);
		_equippedWeapon->setRotation(0);
		_equippedWeapon->setLocalZOrder(1);
	}
}
/***************************************************************************
  函数名称：moveTowards
  功    能：移动至目标地点
  输入参数：const cocos2d::Vec2& targetPosition目标位置
  返 回 值：
  说    明：
***************************************************************************/
void Player::moveTowards(const cocos2d::Vec2& targetPosition)
{
	Vec2 currentPosition = this->getPosition();
	Vec2 directionVector = targetPosition - currentPosition;
	//距离
	float distance = directionVector.getLength();
	//时间
	float duration = distance / this->getMoveSpeed();

	auto action = MoveTo::create(duration, targetPosition);
	this->runAction(action);
}



/***************************************************************************
  函数名称：moveRight
  功    能：向右移动
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void Player::moveRight()
{
	Vec2 currentPosition = this->getPosition();
	Vec2 directionVector(1, 0);
	Vec2 targetPosition = currentPosition + directionVector;
	this->moveTowards(targetPosition);
	return;
}
/***************************************************************************
  函数名称：moveLeft
  功    能：向左移动
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void Player::moveLeft()
{
	Vec2 currentPosition = this->getPosition();
	Vec2 directionVector(-1, 0);
	Vec2 targetPosition = currentPosition + directionVector;
	this->moveTowards(targetPosition);
	return;
}