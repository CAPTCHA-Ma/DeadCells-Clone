#include "DeadArcher.h"
USING_NS_CC;

// 配置常量：定义死亡弓箭手的物理属性与感知范围
const float targetWidth = 150.0f;
const float targetHeight = 150.0f;
const float moveSpeed = 100.0f;      // 移动速度（比掷弹兵稍快）
const float attackRange = 200.0f;    // 射程
const float DetectionRange = 400.0f; // 追踪范围
const BasicAttributes basicAttribute = { 100.0f, 100.0f, 0.0f };

// -----------------------------------------------------------------------------
// 初始化逻辑
// -----------------------------------------------------------------------------
bool DeadArcher::init()
{
    _type = MonsterCategory::Zombie; // 属于僵尸/不死类怪
    if (!Node::init()) return false;

    // 基础视觉组件
    _sprite = Sprite::create("Graph/DeadArcher/idle_00-=-0-=-.png");
    if (!_sprite) return false;
    this->addChild(_sprite);

    // 属性初始化
    this->setCurrentAttributes(basicAttribute);
    this->setMaxHealth(basicAttribute.health);

    _state = DeadArcherState::idle;
    _direction = MoveDirection::RIGHT;
    _moveSpeed = moveSpeed;
    _attackRange = attackRange;

    // 创建碰撞身体
    this->createBody(
        Size(targetWidth / 3.0f, targetHeight / 3.0f),
        Vec2(0, targetHeight / 6.0f)
    );

    this->setupHPBar();      // 必须手动调用初始化血条
    this->scheduleUpdate();  // 开启 update 处理 AI 和缩放逻辑
    playAnimation(DeadArcherState::idle, true);
    return true;
}

/**
 * 死亡回调：处理物理停止与销毁动画
 */
void DeadArcher::onDead()
{
    changeState(DeadArcherState::dead);

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
// 动作行为实现
// -----------------------------------------------------------------------------

void DeadArcher::idle()
{
    if (_body)
    {
        Vec2 currentVel = _body->getVelocity();
        currentVel.x = 0; // 停止水平移动
        _body->setVelocity(Vec2(0, currentVel.y));
    }
}

/**
 * 射击逻辑：包含延迟触发，模拟“拉弓”动作
 */
void DeadArcher::shoot()
{
    this->idle(); // 射击时保持原地不动

    // 延迟 0.5 秒发射，为了对应动画中拉弓完成的时刻
    auto delay = DelayTime::create(0.5f);
    auto doShoot = CallFunc::create([this]() {
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;

        // 利用工厂类 FlyingObject 创建箭矢
        auto arrow = dynamic_cast<Arrow*>(FlyingObject::create(FlyType::Arrow, false, this->getFinalAttack()));
        if (arrow) {
            // 设置箭矢生成的初始位置（大致在弓箭手手部高度）
            arrow->setPosition(this->getPosition() + Vec2(dir * 20, 50));
            this->getParent()->addChild(arrow);
            arrow->run(Vec2(dir, 0)); // 让箭矢沿水平方向飞行
        }
        });

    this->runAction(Sequence::create(delay, doShoot, nullptr));
}

void DeadArcher::walk()
{
    if (_body)
    {
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        if (_sprite) _sprite->setFlippedX(dir == -1);

        Vec2 vel = _body->getVelocity();
        vel.x = _moveSpeed * dir;
        _body->setVelocity(vel);
    }
}

// -----------------------------------------------------------------------------
// AI 逻辑控制
// -----------------------------------------------------------------------------

void DeadArcher::ai(float dt, cocos2d::Vec2 playerWorldPos)
{
    if (_isDead) return;

    // 被攻击硬直判断
    if (this->isStunned())
    {
        if (_state != DeadArcherState::idle) {
            this->changeState(DeadArcherState::idle); // 打断当前动作
        }
        return;
    }

    if (_state == DeadArcherState::shoot) return; // 射击中不干扰

    _aiTickTimer += dt;

    // 计算玩家与怪物的世界坐标距离
    Vec2 myWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
    Vec2 toPlayer = playerWorldPos - myWorldPos;
    float distX = abs(toPlayer.x);
    float distY = abs(toPlayer.y);

    // 每 0.5 秒进行一次决策更新
    if (_aiTickTimer >= 0.5f)
    {
        _aiTickTimer = 0.0f;

        // 判定 1：在射程内且高度基本一致 -> 射击
        if (distX <= _attackRange && distY < 2.0f)
        {
            _direction = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            _sprite->setFlippedX(_direction == MoveDirection::LEFT);
            this->changeState(DeadArcherState::shoot);
        }
        // 判定 2：在侦测范围内但射程外 -> 追踪走过去
        else if (distX <= DetectionRange && distY < 2.0f)
        {
            MoveDirection newDir = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            _direction = newDir;
            this->changeState(DeadArcherState::walk);
        }
        // 判定 3：离得太远 -> 待机
        else
        {
            this->changeState(DeadArcherState::idle);
        }
    }
}

// -----------------------------------------------------------------------------
// 状态机与动画播放
// -----------------------------------------------------------------------------

void DeadArcher::changeState(DeadArcherState newState)
{
    if (_isDead && newState != DeadArcherState::dead) return;
    if (_state == newState) return;

    switch (newState)
    {
        case DeadArcherState::idle:  this->idle(); break;
        case DeadArcherState::shoot: this->shoot(); break;
        case DeadArcherState::walk:  this->walk(); break;
        case DeadArcherState::dead:  this->idle(); break;
        default: break;
    }
    _state = newState;
    playAnimation(newState, DeadArcherStateLoop[newState]);
}
/**
 * 获取动画
 */
cocos2d::Animation* DeadArcher::getAnimation(DeadArcherState state)
{
    auto iter = _animCache.find(state);
    if (iter != _animCache.end())
        return iter->second;

    Animation* anim = nullptr;
    switch (state)
    {
        case DeadArcherState::idle:              anim = createAnim("idle", 46, 1.0f); break;
        case DeadArcherState::shoot:             anim = createAnim("shoot", 22, 1.5f); break;
        case DeadArcherState::walk:              anim = createAnim("walkA", 11, 1.0f); break;
        default:return nullptr;
    }
    anim->retain();
    _animCache[state] = anim;
    return anim;
}
/**
 * 动画资源加载与裁切逻辑
 */
cocos2d::Animation* DeadArcher::createAnim(const std::string& name, int frameCount, float time)
{
    auto anim = Animation::create();
    for (int i = 0; i < frameCount; ++i)
    {
        std::string framePath = StringUtils::format("Graph/DeadArcher/%s_%02d-=-0-=-.png", name.c_str(), i);
        auto tempSprite = Sprite::create(framePath);
        if (!tempSprite) continue;

        auto originalSize = tempSprite->getContentSize();
        // X轴中心偏移计算
        float offsetX = (originalSize.width - targetWidth) / 2.0f;

        // 创建指定尺寸的序列帧
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, 0, targetWidth, targetHeight));
        anim->addSpriteFrame(frame);
    }
    anim->setDelayPerUnit(time / frameCount);
    return anim;
}

/**
 * 播放动画并绑定逻辑（如射击完成后自动回到待机）
 */
void DeadArcher::playAnimation(DeadArcherState state, bool loop)
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
        // 动画序列执行完后，自动切换回 idle 状态
        action = Sequence::create(animate, CallFunc::create([this, state]()
            {
                if (_state == state) changeState(DeadArcherState::idle);
            }), nullptr);
    }

    action->setTag(1001);
    _sprite->runAction(action);
}

/**
 * 近战/身体碰撞判定盒
 */
void DeadArcher::createAttackBox()
{
    removeAttackBox();

    _attackNode = Node::create();
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;
    this->addChild(_attackNode, 10);

    // 设置攻击判定范围
    auto attackBody = PhysicsBody::createBox(
        Size(targetWidth * 2 / 3, targetHeight / 6),
        PhysicsMaterial(0, 0, 0),
        Vec2(dir * targetWidth / 6, targetHeight / 6)
    );

    attackBody->setDynamic(false);
    attackBody->setGravityEnable(false);
    attackBody->setCategoryBitmask(ENEMY_ATTACK);
    attackBody->setCollisionBitmask(0);
    attackBody->setContactTestBitmask(PLAYER_BODY);

    _attackNode->setPhysicsBody(attackBody);

    // 判定框持续 0.5 秒后自动移除
    _attackNode->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() { this->removeAttackBox(); }),
        nullptr
    ));
}