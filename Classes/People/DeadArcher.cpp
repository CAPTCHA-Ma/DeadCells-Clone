#include "DeadArcher.h"
USING_NS_CC;
const float targetWidth = 150.0f;
const float targetHeight = 150.0f;
const float moveSpeed = 100.0f;           //移动速度
const float attackRange = 200.0f;        //攻击范围
const float DetectionRange = 400.0f;    //跟踪范围
const BasicAttributes basicAttribute = {100.0f,100.0f,0.0f };
//*******************************************************************
//*******************************************************************
//*******************************************************************
//初始化
bool DeadArcher::init()
{
    _type = MonsterCategory::Zombie;
    if (!Node::init())
        return false;

    _sprite = Sprite::create("Graph/DeadArcher/idle_00-=-0-=-.png");
    if (!_sprite)
        return false;
    this->addChild(_sprite);
    this->setCurrentAttributes(basicAttribute);
    this->setMaxHealth(basicAttribute.health);
    _state = DeadArcherState::idle;
    _direction = MoveDirection::RIGHT;
    _moveSpeed = moveSpeed;
    _attackRange = attackRange;
    this->createBody(
        Size(targetWidth / 3.0f, targetHeight / 3.0f),
        Vec2(0, targetHeight / 6.0f)
    );
    this->setupHPBar();    // 1. 必须手动调用初始化血条
    this->scheduleUpdate(); // 2. 必须开启 update 才能处理缩放和逻辑
    playAnimation(DeadArcherState::idle, true);
    return true;
}
void DeadArcher::onDead()
{
    changeState(DeadArcherState::dead);

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
void DeadArcher::idle()
{
    if (_body)
    {
        Vec2 currentVel = _body->getVelocity();
        currentVel.x = 0;
        _body->setVelocity(Vec2(0, currentVel.y));
    }
}
void DeadArcher::shoot()
{
    this->idle();
    auto delay = DelayTime::create(0.5f);
    auto doShoot = CallFunc::create([this]() {
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        auto arrow = dynamic_cast<Arrow*>(FlyingObject::create(FlyType::Arrow, false,this->getFinalAttack()));
        if (arrow) {
            arrow->setPosition(this->getPosition() + Vec2(dir * 20, 50)); 
            this->getParent()->addChild(arrow);
            arrow->run(Vec2(dir, 0));
        }
        });

    this->runAction(Sequence::create(delay, doShoot, nullptr));
}
void DeadArcher::walk()
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
void DeadArcher::ai(float dt, cocos2d::Vec2 playerWorldPos)
{
    if (_isDead)
        return;
    if (_state == DeadArcherState::shoot)
        return;

    _aiTickTimer += dt;


    Vec2 myWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
    Vec2 toPlayer = playerWorldPos - myWorldPos;
    float distX = abs(toPlayer.x);
    float distY = abs(toPlayer.y);


    if (_aiTickTimer >= 0.2f)
    {
        _aiTickTimer = 0.0f;
        if (distX <= _attackRange && distY < 2.0f)
        {
            _direction = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
            _sprite->setFlippedX(dir == -1);
            _lastPlayerPos = playerWorldPos;

            this->changeState(DeadArcherState::shoot);
        }
        //跟踪
        else if (distX <= DetectionRange && distY < 2.0f)
        {
            MoveDirection newDir = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            if (_direction != newDir)
            {
                _direction = newDir;
            }
            this->changeState(DeadArcherState::walk);
        }
        else
        {
            this->changeState(DeadArcherState::idle);
        }
    }
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
//动画
void DeadArcher::changeState(DeadArcherState newState)
{
    if (_isDead && newState != DeadArcherState::dead)
        return;

    if (_state == newState)
        return;
    switch (newState)
    {
        case DeadArcherState::idle: this->idle(); break;
        case DeadArcherState::shoot: this->shoot(); break;
        case DeadArcherState::walk: this->walk(); break;
        case DeadArcherState::dead:this->idle(); break;
        default: break;
    }
    _state = newState;
    playAnimation(newState, DeadArcherStateLoop[newState]);
}
cocos2d::Animation* DeadArcher::getAnimation(DeadArcherState state)
{
    auto iter = _animCache.find(state);
    if (iter != _animCache.end())
        return iter->second;

    Animation* anim = nullptr;
    switch (state)
    { 
        case DeadArcherState::idle:              anim = createAnim("idle", 46, 1.0f); break;
        case DeadArcherState::shoot:             anim = createAnim("shoot", 22, 1.0f); break;
        case DeadArcherState::walk:              anim = createAnim("walkA", 11, 1.0f); break;
        default:return nullptr;
    }
    anim->retain();
    _animCache[state] = anim;
    return anim;
}
cocos2d::Animation* DeadArcher::createAnim(const std::string& name, int frameCount, float time)
{
    auto anim = Animation::create();

    // 设置目标裁剪尺寸


    for (int i = 0; i < frameCount; ++i)
    {
        std::string framePath = StringUtils::format("Graph/DeadArcher/%s_%02d-=-0-=-.png", name.c_str(), i);
        auto tempSprite = Sprite::create(framePath);
        if (!tempSprite) 
            continue; 
        auto originalSize = tempSprite->getContentSize();
        float offsetX = (originalSize.width - targetWidth) / 2.0f;
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, 0, targetWidth, targetHeight));
        anim->addSpriteFrame(frame);
    }

    anim->setDelayPerUnit(time / frameCount);
    anim->setRestoreOriginalFrame(false);
    return anim;
}
void DeadArcher::playAnimation(DeadArcherState state, bool loop)
{
    if (!_sprite) 
        return; 
    _sprite->stopActionByTag(1001);
    Animation* anim = getAnimation(state);
    if (!anim) 
        return;
    auto animate = Animate::create(anim);
    Action* action = nullptr;
    if (loop) 
    {
        action = RepeatForever::create(animate);
    }
    else 
    {
        action = Sequence::create(animate, CallFunc::create([this, state]() 
            {
            if (_state == state) 
            {
                changeState(DeadArcherState::idle);
            }
            }), nullptr);
    }

    action->setTag(1001);
    _sprite->runAction(action); // 修改这里：让 _sprite 动起来
}
void DeadArcher::createAttackBox()
{
    removeAttackBox();

    _attackNode = Node::create();
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;
    this->addChild(_attackNode, 10);

    auto attackBody = PhysicsBody::createBox(cocos2d::Size(targetWidth * 2 / 3, targetHeight / 6), PhysicsMaterial(0, 0, 0), Vec2(dir * targetWidth / 6, targetHeight / 6));

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