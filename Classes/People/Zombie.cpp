#include "Zombie.h"
USING_NS_CC;
const float targetWidth = 64.0f;
const float targetHeight = 120.0f;
//*******************************************************************
//*******************************************************************
//*******************************************************************
//初始化
bool Zombie::init()
{
    if (!Sprite::initWithFile("Graph/Zombie/idle-=-0-=-.png"))
        return false;

    // 属性
    _state = ZombieState::idle;
    _direction = MoveDirection::RIGHT;
    _moveSpeed = 150.0f; // 水平移动速度
    _runSpeed = 2 * _moveSpeed;
    _attackRange = 100.0f;

    auto body = PhysicsBody::createBox(cocos2d::Size(targetWidth / 3, targetHeight / 3), PhysicsMaterial(0.1f, 0.0f, 0.5f), Vec2(0, 1.0 / 6 * targetHeight));
    body->setDynamic(true);            // 动态体，受力影响
    body->setRotationEnable(false);    // 禁止旋转
    body->setGravityEnable(true);      // 启用重力
    body->setVelocity(Vec2::ZERO);

    this->setPhysicsBody(body);

    this->getPhysicsBody()->setCategoryBitmask(ENEMY_BODY);//类别掩码1
    this->getPhysicsBody()->setCollisionBitmask(GROUND);//碰撞掩码2

    this->createHurtBox();
    this->scheduleUpdate();
    playAnimation(ZombieState::idle, true);


    return true;
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
//动作
void Zombie::idle()
{
    auto body = this->getPhysicsBody();
    if (body)
    {
        Vec2 currentVel = body->getVelocity();
        currentVel.x = 0;
        body->setVelocity(Vec2(0, currentVel.y));
    }
}
void Zombie::atkA()
{
    createAttackBox();
    this->idle();
}
void Zombie::walk()
{
    auto body = this->getPhysicsBody();
    if (!body) return;

    int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;

    Vec2 vel = body->getVelocity();
    vel.x = _moveSpeed * dir;
    body->setVelocity(vel);

    this->setFlippedX(dir == -1);
}
void Zombie::run()
{
    auto body = this->getPhysicsBody();
    if (!body) return;
    int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
    Vec2 vel = body->getVelocity();
    vel.x = _runSpeed * dir;
    body->setVelocity(vel);
    this->setFlippedX(dir == -1);
}
void Zombie::dead()
{
    this->removeFromParentAndCleanup(true);
}
void Zombie::ai(float dt, cocos2d::Vec2 playerWorldPos)
{
    if (_state == ZombieState::atkA)
        return;

    _aiTickTimer += dt;


    Vec2 myWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
    Vec2 toPlayer = playerWorldPos - myWorldPos;
    float distX = abs(toPlayer.x);
    float distY = abs(toPlayer.y);

    // 2. 决策阶段
    if (_aiTickTimer >= 0.2f)
    {
        _aiTickTimer = 0.0f;

        // 判断 A: 是否在攻击范围内 (水平距离够近 且 高度基本一致)
        // 这里的 _attackRange 建议根据投弹兵特性设定，比如 100-200
        if (distX <= _attackRange && distY < 10.0f)
        {
            // 攻击前修正一次朝向，确保正对玩家
            _direction = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
            this->setFlippedX(dir == -1);
            this->changeState(ZombieState::atkA);
        }
        // 判断 B: 是否在追踪范围内
        else if (distX <= 200.0f && distY < 200.0f)
        {
            MoveDirection newDir = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            if (_direction != newDir)
            {
                _direction = newDir;
            }
            this->changeState(ZombieState::run);
        }
        // 判断 C: 距离太远，歇着
        else
        {
            this->changeState(ZombieState::idle);
        }
    }
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
//动画
void Zombie::changeState(ZombieState newState)
{
    if (_state == newState)
        return;
    switch (newState)
    {
        case ZombieState::idle: this->idle(); break;
        case ZombieState::atkA: this->atkA(); break;
        case ZombieState::walk: this->walk(); break;
        case ZombieState::run: this->run(); break;
        default: break;
    }

    _state = newState;
    playAnimation(newState, ZombieStateLoop[newState]);
}
cocos2d::Animation* Zombie::getAnimation(ZombieState state)
{
    auto iter = _animCache.find(state);
    if (iter != _animCache.end())
        return iter->second;

    Animation* anim = nullptr;
    switch (state)
    {
        case ZombieState::idle:            anim = createAnim("idle", 36, 1.0f); break;
        case ZombieState::atkA:             anim = createAnim("atkA", 9, 1.0f); break;
        case ZombieState::walk:            anim = createAnim("walk", 28, 1.0f); break;
        case ZombieState::run:            anim = createAnim("run", 25, 1.0f); break;
        default:return nullptr;
    }
    anim->retain();
    _animCache[state] = anim;
    return anim;
}
cocos2d::Animation* Zombie::createAnim(const std::string& name, int frameCount, float time)
{
    auto anim = Animation::create();

    // 设置目标裁剪尺寸


    for (int i = 0; i < frameCount; ++i)
    {
        std::string framePath = StringUtils::format("Graph/Zombie/%s-=-%d-=-.png", name.c_str(), i);

        // 先创建一个临时Sprite获取图片原始尺寸
        auto tempSprite = Sprite::create(framePath);
        if (!tempSprite) continue; // 图片不存在则跳过

        auto originalSize = tempSprite->getContentSize();

        // 计算截取偏移，使目标区域居中
        float offsetX = (originalSize.width - targetWidth) / 2.0f;

        // 构建SpriteFrame并添加到动画
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, 0, targetWidth, targetHeight));
        anim->addSpriteFrame(frame);
    }

    anim->setDelayPerUnit(time / frameCount);
    anim->setRestoreOriginalFrame(false);
    return anim;
}
void Zombie::playAnimation(ZombieState state, bool loop)
{
    this->stopActionByTag(1001);
    Animation* anim = getAnimation(state);
    if (!anim) return;

    auto animate = Animate::create(anim);
    Action* action = nullptr;

    if (loop)
    {
        action = RepeatForever::create(animate);
    }
    else
    {
        auto callback = CallFunc::create([this, state]() {
            if (_state == state)
            {
                changeState(ZombieState::idle);
            }
            });
        action = Sequence::create(animate, callback, nullptr);
    }

    action->setTag(1001);
    this->runAction(action);
}

void Zombie::createHurtBox()
{
    // Player::createHurtBox()
    _hurtNode = Node::create();
    auto hurtBody = PhysicsBody::createBox(cocos2d::Size(targetWidth / 3, targetHeight / 3), PhysicsMaterial(0, 0, 0), Vec2(targetWidth / 2, targetHeight * 2 / 3));
    hurtBody->setDynamic(false);
    hurtBody->setGravityEnable(false);
    hurtBody->setRotationEnable(false);
    hurtBody->setCategoryBitmask(PLAYER_HURT);    // PLAYER_HURT
    hurtBody->setCollisionBitmask(0);      // 不产生物理碰撞
    hurtBody->setContactTestBitmask(ENEMY_ATTACK); // ENEMY_HIT

    _hurtNode->setPhysicsBody(hurtBody);
    this->addChild(_hurtNode);
}
void Zombie::createAttackBox()
{
    removeAttackBox();

    _attackNode = Node::create();
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;
    _attackNode->setPosition(Vec2(targetWidth / 2 + dir * targetWidth / 6, targetHeight * 2 / 3));
    this->addChild(_attackNode, 10);

    auto attackBody = PhysicsBody::createBox(cocos2d::Size(targetWidth / 2, targetHeight / 6), PhysicsMaterial(0, 0, 0));

    attackBody->setDynamic(false);
    attackBody->setGravityEnable(false);
    attackBody->setCategoryBitmask(PLAYER_ATTACK);
    attackBody->setCollisionBitmask(0);
    attackBody->setContactTestBitmask(ENEMY_HURT);
    _attackNode->setPhysicsBody(attackBody);

    //延长显示时间以便调试 (例如 0.5s)
    _attackNode->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() { this->removeAttackBox(); }),
        nullptr
    ));
}