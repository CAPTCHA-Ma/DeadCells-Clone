// Player.cpp
#include "Player.h"

USING_NS_CC;

Player::Player()
{

}
Player::~Player()
{

}
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
	if (_currentWeapon)
	{
		if (_mainWeapon && _subWeapon)//有主武器和副武器，把当前武器替换为新武器
		{
			_currentWeapon->removeFromParent();
			_currentWeapon = weapon;
		}
		else if (_mainWeapon)//只有主武器，那么新武器添加到副武器,当前武器切换为副武器
		{
			_subWeapon = weapon;
			_currentWeapon = _subWeapon;
		}
		else//只有副武器，那么新武器添加到主武器,当前武器切换为主武器
		{
			_mainWeapon = weapon;
			_currentWeapon = _mainWeapon;
		}
	}
	else//没有装备武器，直接装备新武器
	{
		_currentWeapon = weapon;
		_mainWeapon = weapon;//默认装备为主武器
	}
	if (_currentWeapon)
	{
		_handNode->addChild(_currentWeapon);
		_currentWeapon->setPosition(Vec2::ZERO);
		_currentWeapon->setRotation(0);
		_currentWeapon->setLocalZOrder(1);
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
void Player::jump()//跳跃
{

}
void Player::crouch()//下蹲
{

}
void Player::roll()//翻滚
{

}
void Player::attack()    // 攻击
{

}
void Player::defend()   // 防御
{

}
void Player::moveFromKeyBoard(char input)//键盘控制移动
{
	switch (input)
	{
		case 'A':moveLeft(); break;
		case 'D':moveRight(); break;
		case 'K':jump(); break;
		case 'J':attack(); break;
		default:break;
	}
}
int Player::getFinalAttackPower() const
{
	return _attack + (_currentWeapon ? _currentWeapon->getAttackPower() : 0);
}
int Player::getFinalDefensePower() const
{
	return _defense + (_currentWeapon ? _currentWeapon->getDefensePower() : 0);
}