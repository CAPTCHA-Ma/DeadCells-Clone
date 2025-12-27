// WeaponNode.h
#ifndef __WEAPONNODE_H__
#define __WEAPONNODE_H__
#include "cocos2d.h"
#include "Weapon.h"
#include "Sword.h"
#include "Bow.h"
#include "Shield.h"
#include "people.h"
#include <string>
#include <map>
class WeaponNode : public cocos2d::Sprite {
public:

    static WeaponNode* create(Weapon* weaponData, cocos2d::Vec2 pos);
    static WeaponNode* createSword(Sword::SwordType type, cocos2d::Vec2 pos);
    static WeaponNode* createBow(Bow::BowType type, cocos2d::Vec2 pos);
    static WeaponNode* createShield(Shield::ShieldType type, cocos2d::Vec2 pos);
    bool init(Weapon* weaponData, cocos2d::Vec2 pos);
    Weapon* pickUp();
    Weapon* getWeapon() const { return _weapon; }
    ~WeaponNode();
    void setPrice(int price);
    int getPrice() const { return _price; }

private:
    Weapon* _weapon; 
    int _price = 0; 
    cocos2d::Label* _priceLabel = nullptr; 

};
#endif