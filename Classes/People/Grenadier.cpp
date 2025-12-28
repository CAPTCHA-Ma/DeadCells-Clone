#include "Grenadier.h"
USING_NS_CC;
const float targetWidth = 140.0f;
const float targetHeight = 200.0f;
const float moveSpeed = 50.0f; //移动速度
const float attackRange = 300.0f;//攻击范围
const float DetectionRange = 500.0f;//跟踪范围
const BasicAttributes basicAttribute = {100.0f,100.0f,0.0f };
//*******************************************************************
//*******************************************************************
//*******************************************************************
//初始化
bool Grenadier::init()
{
    _type = MonsterCategory::Grenadier;
    if (!Node::init())
        return false;
    _sprite = Sprite::create("Graph/Grenadier/idle_00-=-0-=-.png");
    if (!_sprite)
        return false;
    this->addChild(_sprite);
    this->setCurrentAttributes(basicAttribute);
    this->setMaxHealth(basicAttribute.health);
    _state = GrenadierState::idle;
    _direction = MoveDirection::RIGHT;
    _moveSpeed = moveSpeed;
    _attackRange = attackRange;
    this->createBody(
        Size(targetWidth / 3.0f, targetHeight / 3.0f),
        Vec2(0, targetHeight / 6.0f)
    );
    this->setupHPBar();    // 1. 必须手动调用初始化血条
    this->scheduleUpdate(); // 2. 必须开启 update 才能处理缩放和逻辑
    playAnimation(GrenadierState::idle, true);
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
    float power = this->getFinalAttack();
    auto bomb = Bomb::create(power);
    if (!bomb)
        return;
    bomb->setPosition(this->getPosition() + Vec2(0, 40));
    this->getParent()->addChild(bomb);
    Vec2 localTargetPos = this->getParent()->convertToNodeSpace(_lastPlayerPos);
    bomb->run(localTargetPos);
}
void Grenadier::walk()
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
void Grenadier::onDead()
{
    changeState(GrenadierState::dead);
    this->unscheduleUpdate();
    if (auto body = this->getPhysicsBody())
    {
        body->setVelocity(Vec2::ZERO);
        body->setAngularVelocity(0);
        body->setContactTestBitmask(0);
        body->setCollisionBitmask(GROUND);
    }
    this->stopAllActions();
    if (_sprite) 
        _sprite->stopAllActions();
    runAction(Sequence::create(
        FadeOut::create(0.5f),
        RemoveSelf::create(true),
        nullptr
    ));
}
void Grenadier::ai(float dt, cocos2d::Vec2 playerWorldPos)
{
    if (_isDead)
        return;

    if (this->isStunned())
    {
        // 如果当前不在 idle 状态，强制切回 idle (表现为被打断)
        if (_state != GrenadierState::idle) {
            this->changeState(GrenadierState::idle);
        }
        return; // 直接返回，不进行追击或攻击判断
    }

    if (_state == GrenadierState::atk)
        return;

    _aiTickTimer += dt;


    Vec2 myWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
    Vec2 toPlayer = playerWorldPos - myWorldPos;
    float distX = abs(toPlayer.x);
    float distY = abs(toPlayer.y);

 
    if (_aiTickTimer >= 0.5f)
    {
        _aiTickTimer = 0.0f;
        if (distX <= _attackRange && distY < 2.0f)
        {
            _direction = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
            _sprite->setFlippedX(dir == -1);
            _lastPlayerPos = playerWorldPos;

            this->changeState(GrenadierState::atk);
        }
        //跟踪
        else if (distX <= DetectionRange && distY < 2.0f)
        {
            MoveDirection newDir = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            if (_direction != newDir)
            {
                _direction = newDir;
            }
            this->changeState(GrenadierState::walk);
        }
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
    if (_isDead && newState != GrenadierState::dead) return;
    if (_state == newState) return;

    switch (newState)
    {
        case GrenadierState::idle: this->idle(); break;
        case GrenadierState::atk:  this->attack(); break;
        case GrenadierState::walk: this->walk(); break;
        case GrenadierState::dead:
            this->idle();
            break;
        default: break;
    }

    _state = newState;
    playAnimation(newState, GrenadierStateLoop[newState]);
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
    for (int i = 0; i < frameCount; ++i)
    {
        std::string framePath = StringUtils::format("Graph/Grenadier/%s_%02d-=-0-=-.png", name.c_str(), i);
        auto tempSprite = Sprite::create(framePath);
        if (!tempSprite) 
            continue;
        auto originalSize = tempSprite->getContentSize();
        float offsetX = (originalSize.width - targetWidth) / 2.0f;
        float offsetY = (originalSize.height - targetHeight) / 2.0f;
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, offsetY, targetWidth, targetHeight));
        anim->addSpriteFrame(frame);
    }
    anim->setDelayPerUnit(time / frameCount);
    anim->setRestoreOriginalFrame(false);
    return anim;
}
void Grenadier::playAnimation(GrenadierState state, bool loop)
{
    if (!_sprite) return;
    _sprite->stopActionByTag(1001);

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
        if (state == GrenadierState::dead) 
        {
            action = animate;
        }
        else {
            action = Sequence::create(animate, CallFunc::create([this, state]() {
                if (_state == state) changeState(GrenadierState::idle);
                }), nullptr);
        }
    }

    action->setTag(1001);
    _sprite->runAction(action);
}
void Grenadier::createAttackBox()
{
    removeAttackBox();

    _attackNode = Node::create();
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;
    _attackNode->setPosition(Vec2(targetWidth / 2 + dir * targetWidth / 6, targetHeight * 2 / 3));
    this->addChild(_attackNode, 10);

    auto attackBody = PhysicsBody::createBox(cocos2d::Size(targetWidth / 3, targetHeight / 6), PhysicsMaterial(0, 0, 0));

    attackBody->setDynamic(false);
    attackBody->setGravityEnable(false);
    attackBody->setCategoryBitmask(ENEMY_ATTACK);
    attackBody->setCollisionBitmask(0);
    attackBody->setContactTestBitmask(PLAYER_BODY|PLAYER_ARROW);
    _attackNode->setPhysicsBody(attackBody);
    _attackNode->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() { this->removeAttackBox(); }),
        nullptr
    ));
}