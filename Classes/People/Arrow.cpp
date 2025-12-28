#include "Arrow.h"

USING_NS_CC;

// 定义箭矢的逻辑尺寸（用于物理碰撞盒）
const float targetWidth = 50.0f;
const float targetHeight = 25.0f;

/**
 * 静态工厂创建函数。
 * @param fromPlayer 是否由玩家发射（决定了它的攻击目标和物理层级）。
 * @param attackPower 箭矢携带的攻击力数值。
 */
Arrow* Arrow::create(bool fromPlayer, float attackPower)
{
    Arrow* pRet = new(std::nothrow) Arrow();
    if (pRet && pRet->init(fromPlayer, attackPower))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

/**
 * 初始化箭矢属性与物理引擎配置。
 */
bool Arrow::init(bool fromPlayer, float attackPower)
{
    // 加载箭矢资源图片
    if (!Sprite::initWithFile("Graph/FlyingObject/heavyArrow-=-0-=-.png"))
        return false;

    this->_speed = 400.0f;      // 箭矢飞行初速度
    this->_hasHit = false;     // 命中标记位

    // 创建矩形物理碰撞盒
    auto hurtbody = PhysicsBody::createBox(Size(targetWidth * 0.8f, targetHeight),
        PhysicsMaterial(0.1f, 0.0f, 0.0f));

    hurtbody->setDynamic(true);      // 设为动态刚体以检测碰撞
    hurtbody->setGravityEnable(false); // 禁用重力，使箭矢直线飞行

    // -----------------------------------------------------------------------------
    // 物理掩码配置：根据来源设置不同的碰撞与检测逻辑
    // -----------------------------------------------------------------------------
    if (fromPlayer)
    {
        // 玩家的箭：类别为 PLAYER_ARROW，会撞击地面，检测敌方身体和地面
        hurtbody->setCategoryBitmask(PLAYER_ARROW);
        hurtbody->setCollisionBitmask(PhysicsCategory::GROUND);
        hurtbody->setContactTestBitmask(ENEMY_BODY | GROUND);
    }
    else
    {
        // 敌人的箭：类别为 ENEMY_ARROW，检测玩家身体、地面以及玩家的盾牌（用于招架）
        hurtbody->setCategoryBitmask(ENEMY_ARROW);
        hurtbody->setCollisionBitmask(PhysicsCategory::GROUND);
        hurtbody->setContactTestBitmask(PLAYER_BODY | GROUND | SHIELD);
    }

    this->setAttackPower(attackPower);
    this->setPhysicsBody(hurtbody);

    // 防御性逻辑：如果箭矢飞出 8 秒未命中任何物体，自动从场景中移除，防止内存泄漏
    this->runAction(Sequence::create(DelayTime::create(8.0f), RemoveSelf::create(), nullptr));

    return true;
}

/**
 * 启动飞行逻辑。
 * @param targetPos 飞行的方向向量（通常为 Vec2(1, 0) 或 Vec2(-1, 0)）。
 */
void Arrow::run(cocos2d::Vec2 targetPos)
{
    float speed = _speed;
    auto body = this->getPhysicsBody();
    if (body)
        body->setVelocity(targetPos * speed); // 设置物理速度

    // 根据水平移动方向镜像图片
    if (targetPos.x < 0)
        _direction = MoveDirection::LEFT;
    else
        _direction = MoveDirection::RIGHT;

    this->setFlippedX(_direction == MoveDirection::LEFT);
}

/**
 * 处理命中逻辑。
 * 当物理引擎检测到碰撞回调时调用此函数。
 */
void Arrow::hit()
{
    if (_hasHit) return; // 确保只触发一次命中
    _hasHit = true;

    // 停止检测所有碰撞，防止箭矢在消失过程中再次触发伤害
    this->getPhysicsBody()->setContactTestBitmask(0);
    this->getPhysicsBody()->setVelocity(Vec2::ZERO);

    // 播放淡出动画后移除自身
    this->runAction(Sequence::create(
        FadeOut::create(0.2f),
        RemoveSelf::create(),
        nullptr
    ));
}