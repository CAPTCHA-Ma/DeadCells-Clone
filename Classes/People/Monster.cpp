// Monster.cpp
#include "Monster.h"
#include "Player.h"
USING_NS_CC;
/***************************************************************************
  函数名称：initWithType
  功    能：初始化怪物
  输入参数：int monsetrType怪物类型
  返 回 值：bool
  说    明：
***************************************************************************/
bool Monster::initWithType(int monsetrType)
{
	//根据monsterType生成文件名字符串
	std::string filename = StringUtils::format("Monster%d", monsetrType);
	//图片初始化失败
	if (!Sprite::initWithFile(filename))
	{
		return false;
	}
	// 设置初始属性
	this->setHealth(100);
	this->setSpeed(100.0f); // 像素/秒
	return true;
}


/***************************************************************************
  函数名称：createMonster
  功    能：创建怪物
  输入参数：int monsetrType怪物类型
  返 回 值：Monster*
  说    明：
***************************************************************************/
Monster* Monster::createMonster(int monsterType)
{
	Monster* monster = new (std::nothrow)Monster;
	if (monster && monster->initWithType(monsterType))
	{
		monster->autorelease();
		return monster;
	}
	CC_SAFE_DELETE(monster);
	return nullptr;
}



/***************************************************************************
  函数名称：moveTowards
  功    能：移动至目标地点
  输入参数：const cocos2d::Vec2& targetPosition目标位置
  返 回 值：
  说    明：
***************************************************************************/
void Monster::moveTowards(const cocos2d::Vec2& targetPosition)
{
	Vec2 currentPosition = this->getPosition();
	Vec2 directionVector = targetPosition - currentPosition;
	//距离
	float distance = directionVector.getLength();
	//时间
	float duration = distance / this->getSpeed();

	auto action = MoveTo::create(duration, targetPosition);
	this->runAction(action);
}



/***************************************************************************
  函数名称：attack
  功    能：怪物的攻击动作
  输入参数：int monsterType, Player* target
  返 回 值：
  说    明：
***************************************************************************/
void Monster::attack(int monsterType, Player* target)
{

}