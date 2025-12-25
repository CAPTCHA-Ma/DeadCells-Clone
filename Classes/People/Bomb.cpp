#include "Bomb.h"
USING_NS_CC;
const float targetWidth = 50.0f;
const float targetHeight = 50.0f;
bool Bomb::init()
{
	if (!Sprite::initWithFile("Graph/FlyingObject/oilBomb-=-0-=-.png"))
		return false;
	this->_xSpeed = 200.0f;

	auto body = PhysicsBody::createBox(Size(targetWidth, targetHeight), PhysicsMaterial(0, 0, 0));
    body->setDynamic(true);

    body->setCategoryBitmask(PhysicsCategory::ENEMY_BOMB);
    body->setCollisionBitmask(PhysicsCategory::GROUND); 
    body->setContactTestBitmask(PhysicsCategory::PLAYER_BODY | PhysicsCategory::GROUND);

    this->setPhysicsBody(body);
    return true;
}
void Bomb::run(cocos2d::Vec2 targetPos)
{
    auto body = this->getPhysicsBody();
    if (!body) 
        return;
    float g = abs(this->getScene()->getPhysicsWorld()->getGravity().y);
    if (g == 0) 
        g = 980.0f;
    Vec2 currentPos = this->getPosition();
    float dx = targetPos.x - currentPos.x;
    float dy = targetPos.y - currentPos.y;
    float h = 150.0f;
    if (dy > 0) h = dy + 50.0f;
    float vy = sqrt(2 * g * h);
    float discriminant = vy * vy - 2 * g * dy;
    float t = 0.0f;
    if (discriminant >= 0) 
        t = (vy + sqrt(discriminant)) / g;
    else 
        t = 1.0f;
    float vx = dx / t;
    body->setVelocity(Vec2(vx, vy));
    this->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
}
void Bomb::explode()
{
    // 1. 状态锁：防止一弹多炸或重复逻辑
    if (_isExploded) 
        return;
    _isExploded = true;

    // 2. 物理清理：立即停止运动并关掉碰撞监听
    auto body = this->getPhysicsBody();
    if (body) 
    {
        body->setVelocity(Vec2::ZERO);
        body->setContactTestBitmask(0); // 关键：不再产生任何碰撞事件
        body->setGravityEnable(false);  // 炸弹停在空中爆炸
    }

    // 3. 视觉效果：停止旋转，开始爆炸动画
    this->stopAllActions(); // 停止 run() 函数里的 RotateBy

    // 简单的缩放爆炸效果（你可以替换为序列帧动画 Animate）
    auto scaleUp = ScaleTo::create(0.1f, 2.0f);
    auto tint = TintTo::create(0.1f, 255, 100, 100); // 变红
    auto fade = FadeOut::create(0.15f);
    auto remove = RemoveSelf::create();

    // 组合动作
    auto explodeSequence = Sequence::create(
        Spawn::create(scaleUp, tint, nullptr),
        fade,
        remove,
        nullptr
    );

    this->runAction(explodeSequence);
}