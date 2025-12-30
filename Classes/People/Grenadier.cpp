#include "Grenadier.h"
USING_NS_CC;

// 配置常量：定义怪物的视觉尺寸与行为属性
const float targetWidth = 140.0f;
const float targetHeight = 200.0f;
const float moveSpeed = 50.0f;      // 移动速度
const float attackRange = 300.0f;   // 攻击触发范围
const float DetectionRange = 500.0f;// 玩家探测范围
const BasicAttributes basicAttribute = { 100.0f, 100.0f, 0.0f }; // 初始血量、攻击、防御

// -----------------------------------------------------------------------------
// 初始化
// -----------------------------------------------------------------------------
bool Grenadier::init()
{
    _type = MonsterCategory::Grenadier;
    if (!Node::init()) return false;

    // 加载初始帧图片
    _sprite = Sprite::create("Graph/Grenadier/idle_00-=-0-=-.png");
    if (!_sprite) return false;
    this->addChild(_sprite);

    // 设置基础战斗属性
    this->setCurrentAttributes(basicAttribute);
    this->setMaxHealth(basicAttribute.health);

    _state = GrenadierState::idle;
    _direction = MoveDirection::RIGHT;
    _moveSpeed = moveSpeed;
    _attackRange = attackRange;

    // 创建怪物的物理身体（受击盒）
    this->createBody(
        Size(targetWidth / 3.0f, targetHeight / 3.0f),
        Vec2(0, targetHeight / 6.0f)
    );

    this->setupHPBar();      // 初始化血条 UI
    this->scheduleUpdate();  // 开启每帧更新逻辑
    playAnimation(GrenadierState::idle, true); // 播放待机动画

    return true;
}

// -----------------------------------------------------------------------------
// 动作行为实现
// -----------------------------------------------------------------------------

/**
 * 待机：清除水平速度，保持原地不动
 */
void Grenadier::idle()
{
    auto body = this->getPhysicsBody();
    if (body)
    {
        Vec2 currentVel = body->getVelocity();
        body->setVelocity(Vec2(0, currentVel.y)); // 保留重力带来的 Y 轴速度
    }
}

/**
 * 攻击：远程投掷炸弹
 */
void Grenadier::attack()
{
    float power = this->getFinalAttack();
    auto bomb = Bomb::create(power); // 创建炸弹实例
    if (!bomb) return;

    bomb->setPosition(this->getPosition() + Vec2(0, 40)); // 炸弹生成点（手部位置）
    this->getParent()->addChild(bomb);

    // 计算玩家相对于怪物的坐标，启动炸弹飞行逻辑
    Vec2 localTargetPos = this->getParent()->convertToNodeSpace(_lastPlayerPos);
    bomb->run(localTargetPos);
}

/**
 * 行走：根据当前朝向赋予水平速度
 */
void Grenadier::walk()
{
    if (_body)
    {
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        if (_sprite) _sprite->setFlippedX(dir == -1); // 镜像图片

        Vec2 vel = _body->getVelocity();
        vel.x = _moveSpeed * dir;
        _body->setVelocity(vel);
    }
}

/**
 * 死亡处理：清理物理状态并销毁对象
 */
void Grenadier::onDead()
{
    changeState(GrenadierState::dead);

    if (auto body = this->getPhysicsBody())
    {
        body->setVelocity(Vec2::ZERO);
        body->setAngularVelocity(0);
    }

    // 通知 GameScene 删除指针
    auto finishAction = CallFunc::create([this]() {
        this->setReadyToRemove(true);
        });

    runAction(Sequence::create(
        FadeOut::create(0.5f),
        finishAction,
        nullptr
    ));
}

// -----------------------------------------------------------------------------
// AI 核心逻辑
// -----------------------------------------------------------------------------

/**
 * AI 决策：根据玩家位置判断该攻击、追击还是待机
 */
void Grenadier::ai(float dt, cocos2d::Vec2 playerWorldPos)
{
    if (_isDead) return;

    // 眩晕/僵直判断
    if (this->isStunned())
    {
        if (_state != GrenadierState::idle) {
            this->changeState(GrenadierState::idle); // 被打断
        }
        return;
    }

    if (_state == GrenadierState::atk) return; // 攻击动作进行中，不执行其他 AI

    _aiTickTimer += dt;

    // 获取坐标并计算与玩家的距离
    Vec2 myWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
    Vec2 toPlayer = playerWorldPos - myWorldPos;
    float distX = abs(toPlayer.x);
    float distY = abs(toPlayer.y);

    // 降低决策频率（每 0.5 秒决策一次），优化性能
    if (_aiTickTimer >= 0.5f)
    {
        _aiTickTimer = 0.0f;

        // 1. 进入攻击范围：玩家在攻击线内且高度差较小
        if (distX <= _attackRange && distY < 2.0f)
        {
            _direction = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            _sprite->setFlippedX(_direction == MoveDirection::LEFT);
            _lastPlayerPos = playerWorldPos; // 记录玩家位置作为抛射目标

            this->changeState(GrenadierState::atk);
        }
        // 2. 进入探测范围：追击玩家
        else if (distX <= DetectionRange && distY < 2.0f)
        {
            _direction = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            this->changeState(GrenadierState::walk);
        }
        // 3. 范围外：原地待机
        else
        {
            this->changeState(GrenadierState::idle);
        }
    }
}

// -----------------------------------------------------------------------------
// 状态机与动画管理
// -----------------------------------------------------------------------------

/**
 * 状态切换入口
 */
void Grenadier::changeState(GrenadierState newState)
{
    if (_isDead && newState != GrenadierState::dead) return;
    if (_state == newState) return;

    // 执行状态初始化行为
    switch (newState)
    {
        case GrenadierState::idle: this->idle(); break;
        case GrenadierState::atk:  this->attack(); break;
        case GrenadierState::walk: this->walk(); break;
        case GrenadierState::dead: this->idle(); break;
        default: break;
    }

    _state = newState;
    playAnimation(newState, GrenadierStateLoop[newState]);
}

/**
 * 动画缓存管理：避免重复创建动画对象
 */
cocos2d::Animation* Grenadier::getAnimation(GrenadierState state)
{
    auto iter = _animCache.find(state);
    if (iter != _animCache.end()) return iter->second;

    Animation* anim = nullptr;
    switch (state)
    {
        case GrenadierState::idle: anim = createAnim("idle", 40, 1.0f); break;
        case GrenadierState::atk:  anim = createAnim("atk", 15, 1.0f); break;
        case GrenadierState::walk: anim = createAnim("walk", 37, 3.7f); break;
        default: return nullptr;
    }
    anim->retain(); // 保持引用，防止被回收
    _animCache[state] = anim;
    return anim;
}

/**
 * 解析图片序列并创建动画对象，包含帧切片逻辑
 */
cocos2d::Animation* Grenadier::createAnim(const std::string& name, int frameCount, float time)
{
    auto anim = Animation::create();
    for (int i = 0; i < frameCount; ++i)
    {
        std::string framePath = StringUtils::format("Graph/Grenadier/%s_%02d-=-0-=-.png", name.c_str(), i);
        auto tempSprite = Sprite::create(framePath);
        if (!tempSprite) continue;

        // 计算居中切片矩形，确保不同大小的帧序列对齐
        auto originalSize = tempSprite->getContentSize();
        float offsetX = (originalSize.width - targetWidth) / 2.0f;
        float offsetY = (originalSize.height - targetHeight) / 2.0f;
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, offsetY, targetWidth, targetHeight));
        anim->addSpriteFrame(frame);
    }
    anim->setDelayPerUnit(time / frameCount);
    return anim;
}

/**
 * 执行精灵动作播放，处理循环逻辑及动画结束回调
 */
void Grenadier::playAnimation(GrenadierState state, bool loop)
{
    if (!_sprite) return;
    _sprite->stopActionByTag(1001); // 停止旧动画

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
            // 非循环动画结束后自动切回待机状态
            action = Sequence::create(animate, CallFunc::create([this, state]() {
                if (_state == state) changeState(GrenadierState::idle);
                }), nullptr);
        }
    }

    action->setTag(1001);
    _sprite->runAction(action);
}

/**
 * 攻击判定盒（炸弹抛出前的瞬间判定，或用于近身碰撞）
 */
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
    attackBody->setContactTestBitmask(PLAYER_BODY | PLAYER_ARROW);

    _attackNode->setPhysicsBody(attackBody);

    // 判定盒存在 0.5 秒后自动消失
    _attackNode->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() { this->removeAttackBox(); }),
        nullptr
    ));
}