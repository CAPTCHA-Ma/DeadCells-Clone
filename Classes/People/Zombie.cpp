#include "Zombie.h"
USING_NS_CC;

// 配置常量：定义僵尸的尺寸与感知参数
const float targetWidth = 64.0f;
const float targetHeight = 120.0f;
const float moveSpeed = 100.0f;      // 基础移动速度
const float attackRange = 50.0f;     // 近战攻击触发距离
const float DetectionRange = 200.0f; // 发现玩家并开始奔跑的距离
const BasicAttributes basicAttribute = { 100.0f, 100.0f, 0.0f };

// -----------------------------------------------------------------------------
// 初始化逻辑
// -----------------------------------------------------------------------------
bool Zombie::init()
{
    _type = MonsterCategory::Zombie;
    if (!Node::init()) return false;

    // 加载初始精灵图
    _sprite = Sprite::create("Graph/Zombie/idle-=-0-=-.png");
    if (!_sprite) return false;

    this->addChild(_sprite);

    // 设置战斗属性
    this->setCurrentAttributes(basicAttribute);
    this->setMaxHealth(basicAttribute.health);

    // 初始化状态与速度
    _state = ZombieState::idle;
    _direction = MoveDirection::RIGHT;
    _moveSpeed = moveSpeed;
    _runSpeed = 2 * _moveSpeed; // 发现目标时速度翻倍
    _attackRange = attackRange;

    // 创建核心碰撞体
    this->createBody(
        Size(targetWidth / 3.0f, targetHeight / 3.0f),
        Vec2(0, targetHeight / 6.0f)
    );

    this->setupHPBar();      // 初始化血条
    this->scheduleUpdate();  // 启用每帧更新，驱动 AI 行为
    playAnimation(ZombieState::idle, true);

    return true;
}

/**
 * 死亡回调：处理位图淡出与物理移除
 */
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

// -----------------------------------------------------------------------------
// 动作行为实现
// -----------------------------------------------------------------------------

void Zombie::idle()
{
    if (_body)
    {
        Vec2 currentVel = _body->getVelocity();
        currentVel.x = 0; // 水平速度归零
        _body->setVelocity(Vec2(0, currentVel.y));
    }
}

/**
 * 近战攻击：创建瞬时的攻击判定框
 */
void Zombie::atkA()
{
    createAttackBox(); // 在身体前方生成碰撞盒
    this->idle();      // 攻击时停止移动
}

/**
 * 普通行走（巡逻或随机移动）
 */
void Zombie::walk()
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

/**
 * 奔跑（追逐玩家）
 */
void Zombie::run()
{
    if (_body)
    {
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        if (_sprite) _sprite->setFlippedX(dir == -1);

        Vec2 vel = _body->getVelocity();
        vel.x = _runSpeed * dir; // 使用较高的奔跑速度
        _body->setVelocity(vel);
    }
}

// -----------------------------------------------------------------------------
// AI 逻辑控制
// -----------------------------------------------------------------------------

void Zombie::ai(float dt, cocos2d::Vec2 playerWorldPos)
{
    if (_isDead) return;

    // 受击僵直处理：打断当前所有动作并进入待机
    if (this->isStunned())
    {
        if (_state != ZombieState::idle) {
            this->changeState(ZombieState::idle);
        }
        return;
    }

    // 攻击动作执行期间，不处理 AI 逻辑
    if (_state == ZombieState::atkA) return;

    _aiTickTimer += dt;

    // 获取坐标并计算与玩家的距离
    Vec2 myWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
    Vec2 toPlayer = playerWorldPos - myWorldPos;
    float distX = abs(toPlayer.x);
    float distY = abs(toPlayer.y);

    // 每 0.5 秒重新评估一次决策
    if (_aiTickTimer >= 0.5f)
    {
        _aiTickTimer = 0.0f;

        // 判定 1：达到攻击范围（近身）
        if (distX <= _attackRange && distY < 1.0f)
        {
            _direction = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            this->changeState(ZombieState::atkA);
        }
        // 判定 2：在侦测范围内但还够不着 -> 切换到奔跑状态追逐
        else if (distX <= DetectionRange && distY < 1.0f)
        {
            MoveDirection newDir = (toPlayer.x > 0) ? MoveDirection::RIGHT : MoveDirection::LEFT;
            _direction = newDir;
            this->changeState(ZombieState::run);
        }
        // 判定 3：目标丢失或超出范围 -> 待机
        else
        {
            this->changeState(ZombieState::idle);
        }
    }
}

// -----------------------------------------------------------------------------
// 动画与状态管理
// -----------------------------------------------------------------------------

void Zombie::changeState(ZombieState newState)
{
    if (_isDead && newState != ZombieState::dead) return;
    if (_state == newState) return;

    switch (newState)
    {
        case ZombieState::idle: this->idle(); break;
        case ZombieState::atkA: this->atkA(); break;
        case ZombieState::walk: this->walk(); break;
        case ZombieState::run:  this->run(); break;
        case ZombieState::dead: this->idle(); break;
        default: break;
    }
    _state = newState;
    playAnimation(newState, ZombieStateLoop[newState]);
}

/**
 * 序列帧动画创建逻辑
 */
cocos2d::Animation* Zombie::createAnim(const std::string& name, int frameCount, float time)
{
    auto anim = Animation::create();
    for (int i = 0; i < frameCount; ++i)
    {
        // 注意：这里的文件名格式与弓箭手稍有不同，使用了 %s-=-%d-=-.png
        std::string framePath = StringUtils::format("Graph/Zombie/%s-=-%d-=-.png", name.c_str(), i);
        auto tempSprite = Sprite::create(framePath);
        if (!tempSprite) continue;

        auto originalSize = tempSprite->getContentSize();
        float offsetX = (originalSize.width - targetWidth) / 2.0f;
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, 0, targetWidth, targetHeight));
        anim->addSpriteFrame(frame);
    }
    anim->setDelayPerUnit(time / frameCount);
    return anim;
}
/**
 * 获取动画
 */
cocos2d::Animation* Zombie::getAnimation(ZombieState state)
{
    auto iter = _animCache.find(state);
    if (iter != _animCache.end())
        return iter->second;
    Animation* anim = nullptr;
    switch (state)
    {
        case ZombieState::idle:            anim = createAnim("idle", 36, 1.0f); break;
        case ZombieState::atkA:             anim = createAnim("atkA", 9, 1.5f); break;
        case ZombieState::walk:            anim = createAnim("walk", 28, 1.0f); break;
        case ZombieState::run:            anim = createAnim("run", 25, 1.0f); break;
        default:return nullptr;
    }
    anim->retain();
    _animCache[state] = anim;
    return anim;
}
/**
 * 播放动画并处理非循环动画（攻击）的回调
 */
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
            // 攻击动作结束后，如果状态没变（没死或没进新状态），切回待机
            if (_state == state) {
                changeState(ZombieState::idle);
            }
            }), nullptr);
    }
    action->setTag(1001);
    _sprite->runAction(action);
}

/**
 * 创建攻击判定框：在僵尸前方产生一个短暂的物理区域
 */
void Zombie::createAttackBox()
{
    this->removeAttackBox(); // 清除旧的判定框
    this->clearHitTargets(); // 清除本次攻击已命中的目标列表（防止一帧内多次伤害）

    _attackNode = Node::create();
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;
    this->addChild(_attackNode, 10);

    // 碰撞盒位置根据朝向偏移
    auto attackBody = PhysicsBody::createBox(
        Size(targetWidth * 2 / 3, targetHeight / 6),
        PhysicsMaterial(0, 0, 0),
        Vec2(dir * targetWidth / 6, targetHeight / 6)
    );

    attackBody->setDynamic(false);
    attackBody->setGravityEnable(false);
    attackBody->setCategoryBitmask(ENEMY_ATTACK);
    attackBody->setCollisionBitmask(0);
    attackBody->setContactTestBitmask(PLAYER_BODY); // 只检测玩家身体

    _attackNode->setPhysicsBody(attackBody);

    // 判定框存活 0.5 秒后自动消失
    _attackNode->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() { this->removeAttackBox(); }),
        nullptr
    ));
}