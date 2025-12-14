#pragma once
#include "cocos2d.h"
#include "People/Monster.h"
#include "People/Weapon.h"
USING_NS_CC;

enum class Type
{

	combat, corridor, treasure, elite, start, exit

};

class MapUnitData
{
public:

	Type roomtype;
	double x, y;
	int width, height;
	Vector<Monster*> EnemyList;
	Vector<Weapon*> WeaponList;

};

class MapUnit : public cocos2d::Node
{



};