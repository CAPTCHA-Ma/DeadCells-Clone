#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "cocos2d.h"
#include "People.h"
#include "Arrow.h"
#include "Bomb.h"

/**
 * 怪物大类枚举
 */
enum class MonsterCategory
{
    Grenadier,  // 掷弹兵
    Zombie,     // 僵尸
    DeadArcher  // 死亡弓箭手
};

class Monster : public cocos2d::Sprite
{
public:
    // --- 状态查询 ---
    bool isDead() const { return _isDead; }
    bool isReadyToRemove() const { return _isReadyToRemove; }
    bool isStunned() const { return _stunTimer > 0.0f; } // 是否处于受击硬直状态

    // --- 子类必须实现的虚函数 (纯虚函数) ---
    virtual bool init() = 0;
    virtual void idle() = 0;    // 待机行为逻辑
    virtual void walk() = 0;    // 移动行为逻辑
    virtual void onDead() = 0;  // 死亡时的特殊表现（如掉落、特定动画）
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) = 0; // AI 决策核心
    virtual bool isAttackState() = 0; // 当前是否正在攻击

    // --- 通用战斗逻辑 ---
    void struck(float attackPower); // 处理受到伤害
    void dead();                    // 处理死亡逻辑切换
    float getFinalAttack() { return _currentAttributes.attack; };

    // --- 命中目标记录（防止单次攻击产生多段伤害） ---
    void clearHitTargets() { _hitTargets.clear(); }
    void recordHitTarget(cocos2d::Node* target) { _hitTargets.insert(target); }
    bool hasHitTarget(cocos2d::Node* target) const {
        return _hitTargets.find(target) != _hitTargets.end();
    }

    // --- 工厂与属性 ---
    static Monster* create(MonsterCategory category); // 怪物工厂
    MonsterCategory getMonsterType();
    const BasicAttributes& getAttributes() const { return _currentAttributes; }
    void update(float dt);

    // --- 死亡奖励回调 (用于金币/经验发放) ---
    using DeathCallback = std::function<void(int)>;
    void setDeathCallback(DeathCallback cb) { _deathCallback = cb; }
    bool isGoldAwarded() const { return _goldAwarded; }
    void setGoldAwarded(bool awarded) { _goldAwarded = awarded; }

protected:
    // --- 内部组件与状态 ---
    DeathCallback _deathCallback = nullptr;
    bool _goldAwarded = false;
    float _stunTimer = 0.0f;               // 受击硬直计时器
    std::set<cocos2d::Node*> _hitTargets; // 记录当前攻击动作已命中的对象

    // --- 血条系统 ---
    void setupHPBar();
    void updateHPBar();
    cocos2d::DrawNode* _hpBarNode = nullptr;

    // --- 核心属性宏 ---
    CC_SYNTHESIZE(BasicAttributes, _currentAttributes, CurrentAttributes);
    CC_SYNTHESIZE(float, _maxHealth, MaxHealth);

    MonsterCategory _type;
    bool _isDead = false;
    bool _isReadyToRemove = false; // 标记是否可以从内存中彻底删除

    // --- 渲染与物理 ---
    cocos2d::Sprite* _sprite = nullptr;
    cocos2d::PhysicsBody* _body = nullptr;
    cocos2d::Node* _attackNode = nullptr; // 用于存放攻击判定框的节点

    // --- 物理身体与判定逻辑 ---
    virtual void createAttackBox() = 0; // 创建攻击范围（近战/远程不同）
    void removeAttackBox();             // 移除攻击判定
    void createBody(const cocos2d::Size& size, const cocos2d::Vec2& offset); // 创建物理刚体
};

#endif // __MONSTER_H__