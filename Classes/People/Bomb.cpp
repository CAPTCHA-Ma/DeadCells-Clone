#include "Bomb.h"
USING_NS_CC;

// 定义炸弹的碰撞体尺寸
const float targetWidth = 30.0f;
const float targetHeight = 30.0f;

/**
 * 静态工厂函数
 */
Bomb* Bomb::create(float attackPower)
{
    Bomb* pRet = new(std::nothrow) Bomb();
    if (pRet && pRet->init(attackPower))
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
 * 初始化：设置纹理、物理属性和碰撞掩码
 */
bool Bomb::init(float attackPower)
{
    // 加载油弹纹理
    if (!Sprite::initWithFile("Graph/FlyingObject/oilBomb-=-0-=-.png"))
        return false;

    this->_xSpeed = 200.0f;
    this->_isExploded = false;

    // 创建方形物理身体
    auto body = PhysicsBody::createBox(Size(targetWidth, targetHeight), PhysicsMaterial(0, 0, 0));
    body->setDynamic(true); // 必须为动态以受重力影响

    // 物理掩码配置
    body->setCategoryBitmask(PhysicsCategory::ENEMY_BOMB);    // 身份：敌人炸弹
    body->setCollisionBitmask(PhysicsCategory::GROUND);      // 碰撞：仅与地面发生反弹/阻挡
    body->setContactTestBitmask(PhysicsCategory::PLAYER_BODY | PhysicsCategory::GROUND | SHIELD); // 检测：玩家、地面、盾牌

    this->setAttackPower(attackPower);
    this->setPhysicsBody(body);
    return true;
}

/**
 * 核心逻辑：抛物线轨迹计算
 * 使用运动学公式计算到达 targetPos 所需的初速度 (vx, vy)
 */
void Bomb::run(cocos2d::Vec2 targetPos)
{
    auto body = this->getPhysicsBody();
    if (!body) return;

    // 获取物理世界的重力加速度 g
    float g = abs(this->getScene()->getPhysicsWorld()->getGravity().y);
    if (g == 0) g = 980.0f;

    Vec2 currentPos = this->getPosition();
    float dx = targetPos.x - currentPos.x; // 水平位移
    float dy = targetPos.y - currentPos.y; // 垂直位移

    // 设定一个目标高度 h（炸弹升起的最高点）
    float h = 10.0f;
    if (dy > 0) h = dy + 50.0f; // 如果目标比自己高，则最高点需高于目标

    // 1. 根据高度公式计算垂直初速度: vy = sqrt(2 * g * h)
    float vy = sqrt(2 * g * h);

    // 2. 根据位移公式 y = vy*t - 0.5*g*t^2 推导时间 t
    float discriminant = vy * vy - 2 * g * dy;
    float t = 0.0f;
    if (discriminant >= 0)
        t = (vy + sqrt(discriminant)) / g; // 取得落点所需总时间
    else
        t = 1.0f; // 防御性代码

    // 3. 计算水平初速度: vx = dx / t
    float vx = dx / t;

    // 应用速度
    body->setVelocity(Vec2(vx, vy));

    // 让炸弹在飞行过程中不断自转
    this->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
}

/**
 * 爆炸效果逻辑
 */
void Bomb::explode()
{
    if (_isExploded) return;
    _isExploded = true;

    auto body = this->getPhysicsBody();
    if (body)
    {
        body->setVelocity(Vec2::ZERO);    // 停止移动
        body->setContactTestBitmask(0);   // 停止碰撞检测
        body->setGravityEnable(false);    // 爆炸瞬间不受重力影响
    }

    this->stopAllActions(); // 停止旋转动画

    // 视觉表现：变大、变红、淡出
    auto scaleUp = ScaleTo::create(0.1f, 2.0f);     // 快速胀大
    auto tint = TintTo::create(0.1f, 255, 100, 100); // 变红提示火药爆炸
    auto fade = FadeOut::create(0.15f);             // 消失
    auto remove = RemoveSelf::create();

    auto explodeSequence = Sequence::create(
        Spawn::create(scaleUp, tint, nullptr),
        fade,
        remove,
        nullptr
    );

    this->runAction(explodeSequence);
}