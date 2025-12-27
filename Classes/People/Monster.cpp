// Monster.cpp
#include "Grenadier.h"
#include "Zombie.h"
#include "DeadArcher.h"
USING_NS_CC;
void Monster::removeAttackBox()
{
    if (_attackNode)
    {
        _attackNode->removeFromParent();
        _attackNode = nullptr;
    }
}
Monster* Monster::create(MonsterCategory category)
{
    switch (category)
    {
        case MonsterCategory::Grenadier:
            return Grenadier::create();
        case MonsterCategory::Zombie:
            return Zombie::create();
        case MonsterCategory::DeadArcher:
            return DeadArcher::create();
        default:
            return nullptr;
    }
}
MonsterCategory Monster::getMonsterType()
{
    return this->_type;
}


void Monster::createBody(const cocos2d::Size& size, const cocos2d::Vec2& offset)
{
    if (!_body)
    {
        _body = cocos2d::PhysicsBody::create();
        this->setPhysicsBody(_body);
    }
    else
    {
        _body->removeAllShapes();
    }
    float skin = 1.2f;        
    float chamfer = 4.0f;   
    float w = (size.width / 2.0f) - skin;
    float h = (size.height / 2.0f) - skin;
    if (w < 1.0f) w = 1.0f;
    if (h < 1.0f) h = 1.0f;
    if (chamfer > w * 0.5f) chamfer = w * 0.5f;
    if (chamfer > h * 0.5f) chamfer = h * 0.5f;
    cocos2d::Vec2 points[8] = {
        cocos2d::Vec2(-w + chamfer, -h), // 底部左
        cocos2d::Vec2(w - chamfer, -h),  // 底部右
        cocos2d::Vec2(w, -h + chamfer),  // 右侧下
        cocos2d::Vec2(w, h - chamfer),   // 右侧上
        cocos2d::Vec2(w - chamfer, h),   // 顶部右
        cocos2d::Vec2(-w + chamfer, h),  // 顶部左
        cocos2d::Vec2(-w, h - chamfer),  // 左侧上
        cocos2d::Vec2(-w, -h + chamfer)  // 左侧下
    };

    auto material = cocos2d::PhysicsMaterial(0.1f, 0.0f, 0.5f); 
    auto shape = cocos2d::PhysicsShapePolygon::create(points, 8, material, offset);
    _body->addShape(shape);
    _body->setDynamic(true);
    _body->setRotationEnable(false); 
    _body->setGravityEnable(true);
    _body->setVelocity(Vec2::ZERO);
    _body->setCategoryBitmask(ENEMY_BODY);
    _body->setCollisionBitmask(GROUND);
    _body->setContactTestBitmask(PLAYER_ATTACK | PLAYER_ARROW);
}

void Monster::update(float dt)
{
    if (_hpBarNode) {
        _hpBarNode->setScaleX(this->getScaleX() > 0 ? 1.0f : -1.0f);
    }
}
void Monster::setupHPBar()
{
    if (_hpBarNode) return;

    _hpBarNode = cocos2d::DrawNode::create();
    // 获取精灵高度，将血条放在头顶。如果 sprite 为空则给个默认高度
    float spriteHeight = _sprite ? _sprite->getContentSize().height*0.4f : 50.0f;
    _hpBarNode->setPosition(cocos2d::Vec2(0, spriteHeight + 10));

    this->addChild(_hpBarNode, 10);
    updateHPBar();
}

void Monster::updateHPBar()
{
    if (!_hpBarNode) return;
    _hpBarNode->clear();

    float width = 40.0f;  // 血条总宽度
    float height = 5.0f;  // 血条高度
    float percent = (float)_currentAttributes.health / (float)_maxHealth;
    percent = std::max(0.0f, std::min(1.0f, percent));

    // 1. 绘制背景 (黑色半透明)
    _hpBarNode->drawSolidRect(
        cocos2d::Vec2(-width / 2, -height / 2),
        cocos2d::Vec2(width / 2, height / 2),
        cocos2d::Color4F(0, 0, 0, 0.5f)
    );

    // 2. 绘制血量条
    cocos2d::Color4F barColor = (percent > 0.3f) ? cocos2d::Color4F::GREEN : cocos2d::Color4F::RED;
    _hpBarNode->drawSolidRect(
        cocos2d::Vec2(-width / 2, -height / 2),
        cocos2d::Vec2(-width / 2 + (width * percent), height / 2),
        barColor
    );
}
void Monster::struck(float attackPower)
{
    _currentAttributes.health -= attackPower;
    if (_currentAttributes.health <= 0) {
        _currentAttributes.health = 0;
        this->dead();
    }
    updateHPBar();
}
void Monster::dead()
{
    if (_isDead) return;
    _isDead = true;
    this->stopAllActions();
    if (_sprite) 
        _sprite->stopAllActions();


    if (_body) 
    {
        _body->setVelocity(Vec2::ZERO);
        _body->setContactTestBitmask(0);
        _body->setCollisionBitmask(GROUND); 
    }
    auto parentLayer = dynamic_cast<Layer*>(this->getParent());
    if (parentLayer) 
    {
        parentLayer->unscheduleUpdate();
    }

    onDead();
}

