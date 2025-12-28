#include "Weapon.h"
#include "Sword.h"
#include "Shield.h"
#include "Bow.h"

USING_NS_CC;

/**
 * 构造函数
 * @param category 传入武器的大类（剑、盾或弓）
 */
Weapon::Weapon(WeaponCategory category)
	: _category(category)
{
}

/**
 * 武器工厂函数：统一创建接口
 * 根据传入的 Category 创建对应的子类实例
 * * @param category 武器大类枚举
 * @return 返回具体武器子类的基类指针 (多态应用)
 */
Weapon* Weapon::create(WeaponCategory category)
{
	switch (category)
	{
		case WeaponCategory::Sword:
			// 创建默认的第一种剑（索引为0，如匕首）
			return Sword::create(0);

		case WeaponCategory::Shield:
			// 创建默认的第一种盾（索引为0，如闪电盾）
			return Shield::create(0);

		case WeaponCategory::Bow:
			// 创建默认的第一种弓（索引为0，如近战弓）
			return Bow::create(0);

		default:
			// 异常处理：未定义的类别返回空
			return nullptr;
	}
}