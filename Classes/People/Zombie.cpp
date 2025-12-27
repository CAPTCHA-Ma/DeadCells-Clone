#include "Zombie.h"
USING_NS_CC;
const float targetWidth = 64.0f;
const float targetHeight = 120.0f;
const float moveSpeed = 150.0f; //移动速度
const float attackRange = 50.0f; //攻击范围
const float DetectionRange = 200.0f;//跟踪范围
const BasicAttributes basicAttribute = {100.0f,100.0f,0.0f };
//*******************************************************************
//*******************************************************************
//*******************************************************************
//初始化
bool Zombie::init()
{
    _type = MonsterCategory::Zombie;
    if (!Node::init())
        return false;

    _sprite = Sprite::create("Graph/Zombie/idle-=-0-=-.png");
    if (!_sprite)
        return false;

    this->addChild(_sprite);
    this->setCurrentAttributes(basicAttribute);
    this->setMaxHealth(basicAttribute.health);
    _state = ZombieState::idle;
    _direction = MoveDirection::RIGHT;
    _moveSpeed = moveSpeed;
    _runSpeed = 2 * _moveSpeed;
    _attackRange = attackRange;

    this->createBody(
        Size(targetWidth / 3.0f, targetHeight / 3.0f),
        Vec2(0, targetHeight / 6.0f)
    );
    this->setupHPBar();    // 1. 必须手动调用初始化血条
    this->scheduleUpdate(); // 2. 必须开启 update 才能处理缩放和逻辑
    playAnimation(ZombieState::idle, true);
    return true;
}
void Zombie::onDead()
{
    changeState(ZombieState::dead);

    if (auto body = this->getPhysicsBody()) 
    {
        body->setVelocity(Vec2::ZERO);
        body->setAngularVelocity(0);
    }

    runAction(Sequence::create(
        FadeOut::create(0.5f),
        RemoveSelf::create(true),
        nullptr
    ));
}

//*******************************************************************
//*******************************************************************
//*******************************************************************
//动作
void Zombie::idle()
{
    if (_body)
    {
        Vec2 currentVel = _body->getVelocity();
        currentVel.x = 0;
        _body->setVelocity(Vec2(0, currentVel.y));
    }
}
void Zombie::atkA()
{
    createAttackBox();
    this->idle();
}
void Zombie::walk()
{
    if (_body)
    {
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        if (_sprite)
            _sprite->setFlippedX(dir == -1);
        Vec2 vel = _body->getVelocity();
        vel.x = _moveSpeed * dir;
        _body->setVelocity(vel);
    }
}
void Zombie::run()
{
    if (_body)
    {
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        if (_sprite)
            _sprite->setFlippedX(dir == -1);
        Vec2 vel = _body->getVelocity();
        vel.x = _runSpeed * dir;
        _body->setVelocity(vel);
    }
}
void Zombie::ai(float dt, cocos2d::Vec2 playerWorldPos)
{
    if (_isDead)
        return;
    if (_state == ZombieState::atkA)
        return;

    _aiTickTimer += dt;


    Vec2 myWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
    Vec2 toPlayer = playerWorldPos - myWorldPos;
    float distX = abs(toPlayer.x);
    float distY = abs(toPlayer.y);
    if (_aiTickTimer >= 0.2f)
    {
        _aiTickTimer = 0.0f;
        if (distX <= _attackRange && distY < 1.0f)
        {
            _direction = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
            _sprite->setFlippedX(dir == -1);
            this->changeState(ZombieState::atkA);
        }
        // 判断 B: 是否在追踪范围内
        else if (distX <= DetectionRange && distY < 1.0f)
        {
            MoveDirection newDir = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            if (_direction != newDir)
            {
                _direction = newDir;
            }
            this->changeState(ZombieState::run);
        }
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
    if (_isDead && newState != ZombieState::dead)
        return;

    if (_state == newState)
        return;
    switch (newState)
    {
        case ZombieState::idle: this->idle(); break;
        case ZombieState::atkA: this->atkA(); break;
        case ZombieState::walk: this->walk(); break;
        case ZombieState::run:  this->run(); break;
        case ZombieState::dead:this->idle(); break;
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
    if (!_sprite) return;
    _sprite->stopActionByTag(1001);
    Animation* anim = getAnimation(state);
    if (!anim) return;
    auto animate = Animate::create(anim);
    Action* action = nullptr;
    if (loop) {
        action = RepeatForever::create(animate);
    }
    else {
        action = Sequence::create(animate, CallFunc::create([this, state]() {
            if (_state == state) {
                changeState(ZombieState::idle);
            }
            }), nullptr);
    }
    action->setTag(1001);
    _sprite->runAction(action); 
}
void Zombie::createAttackBox()
{
    this->removeAttackBox();
    this->clearHitTargets();
    _attackNode = Node::create();
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;
    this->addChild(_attackNode, 10);
    auto attackBody = PhysicsBody::createBox(cocos2d::Size(targetWidth*2/3, targetHeight / 6), PhysicsMaterial(0, 0, 0),Vec2(dir*targetWidth/6,targetHeight/6));
    attackBody->setDynamic(false);
    attackBody->setGravityEnable(false);
    attackBody->setCategoryBitmask(ENEMY_ATTACK);
    attackBody->setCollisionBitmask(0);
    attackBody->setContactTestBitmask(PLAYER_BODY);
    _attackNode->setPhysicsBody(attackBody);
    _attackNode->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() { this->removeAttackBox(); }),
        nullptr
    ));
}