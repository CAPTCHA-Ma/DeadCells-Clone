#include "WeaponNode.h"
USING_NS_CC;

WeaponNode::~WeaponNode() 
{
    CC_SAFE_DELETE(_weapon);
}
WeaponNode* WeaponNode::createSword(Sword::SwordType type, cocos2d::Vec2 pos)
{
    auto swordData = new (std::nothrow) Sword(type);
    return WeaponNode::create(swordData, pos);
}
WeaponNode* WeaponNode::createBow(Bow::BowType type, cocos2d::Vec2 pos) 
{
    auto bowData = new (std::nothrow) Bow(type);
    return WeaponNode::create(bowData, pos);
}
WeaponNode* WeaponNode::createShield(Shield::ShieldType type, cocos2d::Vec2 pos) 
{
    auto shieldData = new (std::nothrow) Shield(type);
    return WeaponNode::create(shieldData, pos);
}
Weapon* WeaponNode::pickUp() 
{
    Weapon* temp = _weapon;
    _weapon = nullptr; 
    this->removeFromParent();
    return temp;
}
WeaponNode* WeaponNode::create(Weapon* weapon, cocos2d::Vec2 pos)
{
    if (!weapon) 
        return nullptr;
    auto node = new (std::nothrow) WeaponNode();
    if (node && node->init(weapon, pos))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}
bool WeaponNode::init(Weapon* weapon, cocos2d::Vec2 pos)
{
    std::string texturePath = "";

    if (auto sword = dynamic_cast<Sword*>(weapon))
    {
        switch (sword->getSwordType()) 
        {
            case Sword::SwordType::BackStabber: texturePath = "Graph/Weapon/BackStabber.png"; break;
            case Sword::SwordType::BaseballBat: texturePath = "Graph/Weapon/BaseballBat.png"; break;
            case Sword::SwordType::BroadSword:  texturePath = "Graph/Weapon/BroadSword.png";  break;
            case Sword::SwordType::OvenAxe:     texturePath = "Graph/Weapon/OvenAxe.png";     break;
        }
    }
    else if (auto bow = dynamic_cast<Bow*>(weapon))
    {
        switch (bow->getBowType())
        {
            case Bow::BowType::closeCombatBow: texturePath = "Graph/Weapon/closeCombatBow.png"; break;
            case Bow::BowType::dualBow:        texturePath = "Graph/Weapon/dualBow.png";  break;
            case Bow::BowType::crossbow:       texturePath = "Graph/Weapon/crossbow.png"; break;
        }
    }
    else if (auto shield = dynamic_cast<Shield*>(weapon))
    {
        switch (shield->getShieldType())
        {
            case Shield::ShieldType::LightningShield: texturePath = "Graph/Weapon/LightningShield.png"; break;
            case Shield::ShieldType::ParryShield:     texturePath = "Graph/Weapon/ParryShield.png"; break;
        }
    }

    // 2. 初始化 Sprite
    if (!Sprite::initWithFile(texturePath))
    {
        Sprite::initWithFile("Graph/Weapon/BackStabber.png");
    }


    this->_weapon = weapon;
    this->setPosition(pos);

    // 设置物理体
    auto body = PhysicsBody::createBox(this->getContentSize());
    body->setDynamic(false);
    body->setCategoryBitmask(WEAPON);   
    body->setCollisionBitmask(0);             // 不阻挡角色走动
    body->setContactTestBitmask(PLAYER_BODY); // 允许触发碰撞回调
    this->setPhysicsBody(body);

    // 添加表现动画
    auto moveUp = MoveBy::create(1.0f, Vec2(0, 8));
    this->runAction(RepeatForever::create(Sequence::create(moveUp, moveUp->reverse(), nullptr)));

    return true;
}