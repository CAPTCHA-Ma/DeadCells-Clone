#include "Grenadier.h"
USING_NS_CC;
//*******************************************************************
//*******************************************************************
//*******************************************************************
//初始化
bool Grenadier::init()
{
    if (!Sprite::initWithFile("Graph/Grenadier/grenadierInit.png"))
        return false;

    // 属性
    _state = GrenadierState::idle;
    _direction = MoveDirection::RIGHT;
    _moveSpeed = 150.0f; // 水平移动速度
    _attackRange = 100.0f;

    Size bodySize = Size(40, 75);

    float bottomPadding = 100.0f;
    float offsetY = -(this->getContentSize().height / 2 - bodySize.height / 2 - bottomPadding);

    auto body = PhysicsBody::createBox(bodySize,
        PhysicsMaterial(0.1f, 0.0f, 0.5f),
        Vec2(0, offsetY));

    body->setDynamic(true);            // 动态体，受力影响
    body->setRotationEnable(false);    // 禁止旋转
    body->setGravityEnable(true);      // 启用重力
    body->setVelocity(Vec2::ZERO);

    this->setPhysicsBody(body);

    this->getPhysicsBody()->setCategoryBitmask(ENEMY_BODY);//类别掩码1
    this->getPhysicsBody()->setCollisionBitmask(GROUND);//碰撞掩码2


    this->scheduleUpdate();
    playAnimation(GrenadierState::idle, true);

    this->createHurtBox();


    return true;
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
//动作
void Grenadier::idle()
{
    auto body = this->getPhysicsBody();
    if (body)
    {
        Vec2 currentVel = body->getVelocity();
        currentVel.x = 0;
        body->setVelocity(Vec2(0, currentVel.y));
    }
}
void Grenadier::attack()
{
    createAttackBox();
    this->idle();
}
void Grenadier::walk()
{
    auto body = this->getPhysicsBody();
    if (!body) return;

    int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;

    Vec2 vel = body->getVelocity();
    vel.x = _moveSpeed * dir;
    body->setVelocity(vel);

    this->setFlippedX(dir == -1);
}
void Grenadier::dead()
{
    this->removeFromParentAndCleanup(true);
}
void Grenadier::ai(float dt, cocos2d::Vec2 playerWorldPos)
{
    if (_state == GrenadierState::atk)
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
            this->changeState(GrenadierState::atk);
        }
        // 判断 B: 是否在追踪范围内
        else if (distX <= 200.0f && distY < 200.0f)
        {
            MoveDirection newDir = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            if (_direction != newDir)
            {
                _direction = newDir;
            }
            this->changeState(GrenadierState::walk);
        }
        // 判断 C: 距离太远，歇着
        else
        {
            this->changeState(GrenadierState::idle);
        }
    }
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
//动画
void Grenadier::changeState(GrenadierState newState)
{
    if (_state == newState)
        return;
    switch (newState)
    {
        case GrenadierState::idle: this->idle(); break;
        case GrenadierState::atk: this->attack(); break;
        case GrenadierState::walk: this->walk(); break;
        default: break;
    }

    _state = newState;
    playAnimation(newState, StateLoop[newState]);
}
cocos2d::Animation* Grenadier::getAnimation(GrenadierState state)
{
    auto iter = _animCache.find(state);
    if (iter != _animCache.end())
        return iter->second;

    Animation* anim = nullptr;
    switch (state)
    {
        case GrenadierState::idle:            anim = createAnim("idle", 40, 1.0f); break;
        case GrenadierState::atk:             anim = createAnim("atk", 15, 1.0f); break;
        case GrenadierState::walk:            anim = createAnim("walk", 37, 3.7f); break;
        default:return nullptr;
    }
    anim->retain();
    _animCache[state] = anim;
    return anim;
}
cocos2d::Animation* Grenadier::createAnim(const std::string& name, int frameCount, float time)
{
    auto anim = Animation::create();

    // 设置目标裁剪尺寸
    const float targetWidth = 140.0f;
    const float targetHeight = 200.0f;

    for (int i = 0; i < frameCount; ++i)
    {
        std::string framePath = StringUtils::format("Graph/Grenadier/%s_%02d-=-0-=-.png", name.c_str(), i);

        // 先创建一个临时Sprite获取图片原始尺寸
        auto tempSprite = Sprite::create(framePath);
        if (!tempSprite) continue; // 图片不存在则跳过

        auto originalSize = tempSprite->getContentSize();

        // 计算截取偏移，使目标区域居中
        float offsetX = (originalSize.width - targetWidth) / 2.0f;
        float offsetY = (originalSize.height - targetHeight) / 2.0f;

        // 构建SpriteFrame并添加到动画
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, offsetY, targetWidth, targetHeight));
        anim->addSpriteFrame(frame);
    }

    anim->setDelayPerUnit(time / frameCount);
    anim->setRestoreOriginalFrame(false);
    return anim;
}
void Grenadier::playAnimation(GrenadierState state, bool loop)
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
                changeState(GrenadierState::idle);
            }
            });
        action = Sequence::create(animate, callback, nullptr);
    }

    action->setTag(1001);
    this->runAction(action);
}

void Grenadier::createHurtBox() 
{
    // Player::createHurtBox()
    _hurtNode = Node::create();
    auto hurtBody = PhysicsBody::createBox(Size(40, 75));
    _hurtNode->setPosition(Vec2(70, 137));
    hurtBody->setDynamic(false);
    hurtBody->setGravityEnable(false);
    hurtBody->setRotationEnable(false);
    hurtBody->setCategoryBitmask(PLAYER_HURT);    // PLAYER_HURT
    hurtBody->setCollisionBitmask(0);      // 不产生物理碰撞
    hurtBody->setContactTestBitmask(ENEMY_ATTACK); // ENEMY_HIT

    _hurtNode->setPhysicsBody(hurtBody);
    this->addChild(_hurtNode);
}
void Grenadier::createAttackBox()
{
    removeAttackBox();

    _attackNode = Node::create();
    _attackNode->setPosition(Vec2(75, 75));
    this->addChild(_attackNode, 10);

    //获取方向偏移
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;


    float boxWidth = 80.0f;
    float boxHeight = 50.0f;
    float offsetX = 20.0f * dir; // 向右为 +40，向左为 -40

    // 计算矩形的左下角和右上角
    Vec2 origin = Vec2(offsetX - boxWidth / 2, -boxHeight / 2);
    Vec2 destination = Vec2(offsetX + boxWidth / 2, boxHeight / 2);


    auto attackBody = PhysicsBody::createBox(Size(boxWidth, boxHeight),
        PhysicsMaterial(0, 0, 0),
        Vec2(offsetX, 35));
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