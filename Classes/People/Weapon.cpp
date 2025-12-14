// Weapon.cpp
#include "Weapon.h"

USING_NS_CC;



bool Weapon::init(Category weaponCategory, int subTypeIndex)
{
	_category = weaponCategory;
	_subTypeIndex = subTypeIndex;
	std::string filename;
	switch (_category)
	{
		case Category::Sword:
			switch (static_cast<SwordType>(subTypeIndex)) 
            {
				case SwordType::LongSword:
                    filename = "long_sword.png";
                    _attackPower = 35; 
                    break;
				case SwordType::ShortSword: 
                    filename = "short_sword.png"; 
                    _attackPower = 25;
                    break;
				case SwordType::GreatSword: filename = "great_sword.png"; 
                    _attackPower = 50;
                    break;
			}
			_defensePower = 0;
			break;
        case Category::Shield:
            switch (static_cast<ShieldType>(subTypeIndex)) 
            {
                case ShieldType::SmallShield:
                    filename = "small_shield.png"; 
                    _defensePower = 20;
                    break;
                case ShieldType::IronShield: 
                    filename = "iron_shield.png";
                    _defensePower = 40; 
                    break;
                case ShieldType::MagicShield:
                    filename = "magic_shield.png";
                    _defensePower = 50; 
                    break;
            }
            _attackPower = 5;
            break;

        case Category::Bow:
            switch (static_cast<BowType>(subTypeIndex)) 
            {
                case BowType::ShortBow: 
                    filename = "short_bow.png";
                    _attackPower = 20; 
                    break;
                case BowType::LongBow:
                    filename = "long_bow.png"; 
                    _attackPower = 30;
                    break;
                case BowType::CompositeBow:
                    filename = "composite_bow.png";
                    _attackPower = 40;
                    break;
            }
            _defensePower = 0;
            break;
    }

    if (!Sprite::initWithFile(filename))
    {
        return false;
    }

    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    return true;
}

Weapon* Weapon::create(Category weaponCategory, int subTypeIndex)
{
    Weapon* weapon = new (std::nothrow)Weapon;
    if (weapon && weapon->init(weaponCategory, subTypeIndex))
    {
        weapon->autorelease();
        return weapon;
    }
    CC_SAFE_DELETE(weapon);
    return nullptr;
}