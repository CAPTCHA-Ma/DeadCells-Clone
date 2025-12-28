#include "Grenadier.h"
#include "Zombie.h"
#include "DeadArcher.h"

USING_NS_CC;

/**
 * 移除攻击判定框
 * 当攻击动作结束或怪物被打断（硬直）时调用，确保不会产生残留伤害
 */
void Monster::removeAttackBox()
{
    if (_attackNode)
    {
        _attackNode->removeFromParent();
        _attackNode = nullptr;
    }
}

/**
 * 简单工厂：创建具体的怪物实例
 * @param category 怪物分类枚举
 */
Monster* Monster::create(MonsterCategory category)
{
    switch (category)
    {
        case MonsterCategory::Grenadier:  return Grenadier::create();
        case MonsterCategory::Zombie:     return Zombie::create();
        case MonsterCategory::DeadArcher: return DeadArcher::create();
        default: return nullptr;
    }
}

MonsterCategory Monster::getMonsterType()
{
    return this->_type;
}

/**
 * 创建怪物的物理身体
 * 使用 8 个点定义一个切角矩形（Chamfered Rect），使边缘更圆滑，防止移动时卡住
 */
void Monster::createBody(const cocos2d::Size& size, const cocos2d::Vec2& offset)
{
    if (!_body)
    {
        _body = cocos2d::PhysicsBody::create();
        this->setPhysicsBody(_body);
    }
    else
    {
        _body->removeAllShapes();
    }

    // 物理形状微调参数
    float skin = 1.2f;      // 缩进值，防止贴图与物理边界过于紧凑
    float chamfer = 4.0f;   // 切角尺寸
    float w = (size.width / 2.0f) - skin;
    float h = (size.height / 2.0f) - skin;

    // 防止无效尺寸
    if (w < 1.0f) w = 1.0f;
    if (h < 1.0f) h = 1.0f;
    if (chamfer > w * 0.5f) chamfer = w * 0.5f;
    if (chamfer > h * 0.5f) chamfer = h * 0.5f;

    // 定义 8 个顶点（顺时针或逆时针），构建切角多边形
    cocos2d::Vec2 points[8] = {
        cocos2d::Vec2(-w + chamfer, -h), // 底部左
        cocos2d::Vec2(w - chamfer, -h),  // 底部右
        cocos2d::Vec2(w, -h + chamfer),  // 右侧下
        cocos2d::Vec2(w, h - chamfer),   // 右侧上
        cocos2d::Vec2(w - chamfer, h),   // 顶部右
        cocos2d::Vec2(-w + chamfer, h),  // 顶部左
        cocos2d::Vec2(-w, h - chamfer),  // 左侧上
        cocos2d::Vec2(-w, -h + chamfer)  // 左侧下
    };

    auto material = cocos2d::PhysicsMaterial(0.1f, 0.0f, 0.5f); // 密度, 弹性, 摩擦力
    auto shape = cocos2d::PhysicsShapePolygon::create(points, 8, material, offset);

    _body->addShape(shape);
    _body->setDynamic(true);
    _body->setRotationEnable(false); // 禁止旋转（防止怪物像球一样滚走）
    _body->setGravityEnable(true);
    _body->setVelocity(Vec2::ZERO);

    // 物理掩码：定义身份和碰撞规则
    _body->setCategoryBitmask(ENEMY_BODY);
    _body->setCollisionBitmask(GROUND | PLATFORM | MIX);          // 只与地形碰撞
    _body->setContactTestBitmask(PLAYER_ATTACK | PLAYER_ARROW);   // 检测玩家攻击
}

/**
 * 每帧更新逻辑
 */
void Monster::update(float dt)
{
    // 1. 处理血条朝向：血条不随精灵翻转而反向
    if (_hpBarNode) {
        _hpBarNode->setScaleX(this->getScaleX() > 0 ? 1.0f : -1.0f);
    }

    // 2. 更新受击硬直计时器
    if (_stunTimer > 0.0f)
    {
        _stunTimer -= dt;
        if (_stunTimer < 0.0f) _stunTimer = 0.0f;
    }
}

/**
 * 初始化血条组件（DrawNode）
 */
void Monster::setupHPBar()
{
    if (_hpBarNode) return;

    _hpBarNode = cocos2d::DrawNode::create();
    // 放在怪物头顶 (基于 Sprite 高度的一定比例)
    float spriteHeight = _sprite ? _sprite->getContentSize().height * 0.4f : 50.0f;
    _hpBarNode->setPosition(cocos2d::Vec2(0, spriteHeight + 10));

    this->addChild(_hpBarNode, 10);
    updateHPBar();
}

/**
 * 绘制血条内容
 */
void Monster::updateHPBar()
{
    if (!_hpBarNode) return;
    _hpBarNode->clear();

    float width = 40.0f;  // 总宽
    float height = 5.0f;  // 高度
    float percent = (float)_currentAttributes.health / (float)_maxHealth;
    percent = std::max(0.0f, std::min(1.0f, percent));

    // 背景 (半透明黑)
    _hpBarNode->drawSolidRect(
        cocos2d::Vec2(-width / 2, -height / 2),
        cocos2d::Vec2(width / 2, height / 2),
        cocos2d::Color4F(0, 0, 0, 0.5f)
    );

    // 血条颜色 (健康为绿，危急为红)
    cocos2d::Color4F barColor = (percent > 0.3f) ? cocos2d::Color4F::GREEN : cocos2d::Color4F::RED;
    _hpBarNode->drawSolidRect(
        cocos2d::Vec2(-width / 2, -height / 2),
        cocos2d::Vec2(-width / 2 + (width * percent), height / 2),
        barColor
    );
}

/**
 * 受击处理
 * @param attackPower 来自玩家的伤害值
 */
void Monster::struck(float attackPower)
{
    if (_isDead) return;

    _stunTimer = 0.3f; // 产生 0.3 秒硬直

    this->removeAttackBox(); // 被打时立刻取消当前攻击判定

    if (_body) {
        // 水平速度强制归零，模拟受击停顿效果
        _body->setVelocity(Vec2(0, _body->getVelocity().y));
    }

    _currentAttributes.health -= attackPower;
    if (_currentAttributes.health <= 0) {
        _currentAttributes.health = 0;
        this->dead();
    }
    updateHPBar(); // 同步血条 UI
}

/**
 * 死亡统一处理：清理物理状态，调用具体的 onDead 动画逻辑
 */
void Monster::dead()
{
    if (_isDead) return;
    _isDead = true;

    this->stopAllActions();
    if (_sprite) _sprite->stopAllActions();

    if (_body)
    {
        _body->setVelocity(Vec2::ZERO);
        _body->setContactTestBitmask(0); // 死亡后不再受击
        _body->setCollisionBitmask(GROUND | PLATFORM | MIX); // 仅保留地形碰撞逻辑
    }

    onDead(); // 这是一个虚函数，由子类（如 Zombie.cpp）实现具体的消失动画
}