#include "WeaponNode.h"
USING_NS_CC;

/**
 * 析构函数：负责释放逻辑对象 Weapon 的内存
 * 因为 Weapon 不是 Node，不参与 Cocos2d-x 的引用计数，需要手动管理
 */
WeaponNode::~WeaponNode()
{
    CC_SAFE_DELETE(_weapon);
}

// -----------------------------------------------------------------------------
// 快捷工厂方法：创建不同大类的武器节点
// -----------------------------------------------------------------------------

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

/**
 * 通用创建函数：完成 WeaponNode 的实例化与初始化
 */
WeaponNode* WeaponNode::create(Weapon* weapon, cocos2d::Vec2 pos)
{
    if (!weapon) return nullptr;
    auto node = new (std::nothrow) WeaponNode();
    if (node && node->init(weapon, pos))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

/**
 * 初始化逻辑：
 * 1. 使用 dynamic_cast 识别武器具体类型，从而加载对应的资源图片路径。
 * 2. 设置物理碰撞体（Trigger 触发器模式）。
 */
bool WeaponNode::init(Weapon* weapon, cocos2d::Vec2 pos)
{
    std::string texturePath = "";

    // 识别武器类型并匹配贴图
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

    // 初始化精灵图片
    if (!Sprite::initWithFile(texturePath))
    {
        Sprite::initWithFile("Graph/Weapon/BackStabber.png"); // 缺省备选贴图
    }

    this->_weapon = weapon;
    this->setPosition(pos);

    // 设置物理属性：用于玩家靠近检测
    auto body = PhysicsBody::createBox(this->getContentSize());
    body->setGravityEnable(false); // 悬浮状态
    body->setDynamic(false);       // 静态传感器
    body->setCategoryBitmask(WEAPON);
    body->setCollisionBitmask(GROUND);
    body->setContactTestBitmask(PLAYER_BODY | GROUND);
    this->setPhysicsBody(body);

    return true;
}

/**
 * 捡起逻辑：
 * 将底层的 Weapon 数据对象所有权转交给调用者（通常是 PlayerLayer），并销毁自身节点。
 */
Weapon* WeaponNode::pickUp()
{
    Weapon* temp = _weapon;
    _weapon = nullptr; // 释放所有权，防止析构函数将其销毁
    this->removeFromParent();
    return temp;
}

/**
 * 价格标签处理：
 * 用于商店系统，在武器上方显示金币数值标签。
 */
void WeaponNode::setPrice(int price)
{
    _price = price;

    if (_price > 0)
    {
        if (!_priceLabel)
        {
            _priceLabel = Label::createWithTTF(std::to_string(_price), "fonts/fusion-pixel.ttf", 16);
            _priceLabel->setPosition(Vec2(this->getContentSize().width / 2, this->getContentSize().height + 20));
            _priceLabel->setColor(Color3B::YELLOW);
            _priceLabel->enableOutline(Color4B::BLACK, 1);
            this->addChild(_priceLabel);
        }
        else
        {
            _priceLabel->setString(std::to_string(_price));
        }
    }
    else if (_priceLabel)
    {
        _priceLabel->removeFromParent();
        _priceLabel = nullptr;
    }
}