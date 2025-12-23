#include "Grenadier.h"
const float targetWidth = 140.0f;
const float targetHeight = 200.0f;
USING_NS_CC;
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
    this->setMonsterAttributes({ 100,100,0 });
    _state = GrenadierState::idle;
    _direction = MoveDirection::RIGHT;
    _moveSpeed = 150.0f;
    _attackRange = 1000.0f;

    _body = PhysicsBody::createBox(Size(targetWidth / 3, targetHeight / 3),PhysicsMaterial(0.1f, 0.0f, 0.5f),Vec2(0, targetHeight / 6));

    _body->setDynamic(true);
    _body->setRotationEnable(false);
    _body->setGravityEnable(true);
    _body->setVelocity(Vec2::ZERO);

    this->setPhysicsBody(_body);

    _body->setCategoryBitmask(ENEMY_BODY);
    _body->setCollisionBitmask(GROUND);
    _body->setContactTestBitmask(PLAYER_ATTACK);


    this->createHurtBox();
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
    auto bomb = Bomb::create();
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
    // 1. 确保切换到死亡状态
    changeState(GrenadierState::dead);

    // 2. 彻底停止父层级对 AI 的调度 (如果有调用的话)
    this->unscheduleUpdate();

    // 3. 物理修复：防止穿墙和死后滑动
    if (auto body = this->getPhysicsBody()) {
        body->setVelocity(Vec2::ZERO);
        body->setAngularVelocity(0);

        // 停止伤害检测，但保留与地面的碰撞防止掉入墙中
        body->setContactTestBitmask(0);
        body->setCollisionBitmask(GROUND);
    }

    // 4. 彻底停止所有残留动作
    this->stopAllActions();
    if (_sprite) _sprite->stopAllActions();

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
    if (_state == GrenadierState::atk)
        return;

    _aiTickTimer += dt;


    Vec2 myWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
    Vec2 toPlayer = playerWorldPos - myWorldPos;
    float distX = abs(toPlayer.x);
    float distY = abs(toPlayer.y);

 
    if (_aiTickTimer >= 0.2f)
    {
        _aiTickTimer = 0.0f;
        if (distX <= _attackRange && distY < 10.0f)
        {
            _direction = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
            _sprite->setFlippedX(dir == -1);
            _lastPlayerPos = playerWorldPos;

            this->changeState(GrenadierState::atk);
        }
        else if (distX <= 200.0f && distY < 200.0f)
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

    // 设置目标裁剪尺寸


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
        // 死亡状态不需要回调 idle，播完就停在最后一帧等待 Scene 移除
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

void Grenadier::createHurtBox() 
{
    // Player::createHurtBox()
    _hurtNode = Node::create();
    auto hurtBody = PhysicsBody::createBox(cocos2d::Size(targetWidth / 3, targetHeight / 3), PhysicsMaterial(0, 0, 0), Vec2(0,targetHeight/6));
    hurtBody->setDynamic(false);
    hurtBody->setGravityEnable(false);
    hurtBody->setRotationEnable(false);
    hurtBody->setCategoryBitmask(ENEMY_HURT);    // PLAYER_HURT
    hurtBody->setCollisionBitmask(0);      // 不产生物理碰撞
    hurtBody->setContactTestBitmask(PLAYER_ATTACK); // ENEMY_HIT

    _hurtNode->setPhysicsBody(hurtBody);
    this->addChild(_hurtNode);
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
    attackBody->setContactTestBitmask(PLAYER_HURT);
    _attackNode->setPhysicsBody(attackBody);

    //延长显示时间以便调试 (例如 0.5s)
    _attackNode->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() { this->removeAttackBox(); }),
        nullptr
    ));
}