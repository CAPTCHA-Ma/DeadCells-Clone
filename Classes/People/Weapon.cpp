// Weapon.cpp
#include "Weapon.h"
#include "Sword.h"
#include "Shield.h"
#include "Bow.h"
USING_NS_CC;

Weapon::Weapon(WeaponCategory category)
	: _category(category) 
{}

Weapon* Weapon::create(WeaponCategory category)
{
	switch (category)
	{
		case WeaponCategory::Sword:
			return Sword::create(0); // 默认子类型索引为0
		case WeaponCategory::Shield:
			return Shield::create(0); // 默认子类型索引为0
		case WeaponCategory::Bow:
			return Bow::create(0); // 默认子类型索引为0
		default:
			return nullptr;
	}
}