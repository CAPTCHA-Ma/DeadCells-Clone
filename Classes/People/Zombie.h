#ifndef __ZOMBIE_H__
#define __ZOMBIE_H__

#include "Monster.h"
#include <unordered_map>

/**
 * 僵尸状态枚举
 */
enum class ZombieState
{
    idle,   // 待机
    atkA,   // 普通攻击（近战挥砍/咬）
    walk,   // 悠闲行走（巡逻）
    run,    // 快速奔跑（发现目标后的追击）
    dead    // 死亡
};

/**
 * 状态动画循环配置表
 */
static std::unordered_map<ZombieState, bool> ZombieStateLoop =
{
    { ZombieState::idle, true  },
    { ZombieState::atkA,  false }, // 攻击动作播放一次
    { ZombieState::walk,  true  },
    { ZombieState::run,   true  },
    { ZombieState::dead,  false }  // 死亡动作播放一次
};

/**
 * Zombie 类：近战冲锋型怪物
 */
class Zombie : public Monster
{
public:
    // Cocos2d-x 标准创建宏
    CREATE_FUNC(Zombie);

    virtual bool init() override;

    // --- 核心行为重写 ---
    virtual void idle() override;
    virtual void walk() override;
    virtual void onDead() override;

    /**
     * 判断是否处于攻击状态
     * 确保僵尸在挥爪时不会被 AI 逻辑打断而突然开始跑步
     */
    virtual bool isAttackState() override { return _state == ZombieState::atkA; };

    void atkA(); // 具体的近战攻击逻辑
    void run();  // 追击行为逻辑

    /**
     * AI 决策核心
     * 逻辑参考：
     * 1. 玩家太远 -> Walk (巡逻)
     * 2. 玩家进入视线 -> Run (冲刺追击)
     * 3. 玩家进入 _attackRange -> atkA (发起攻击)
     */
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) override;

    // --- 状态与动画管理 ---
    void changeState(ZombieState newState);
    void playAnimation(ZombieState state, bool loop);

private:
    // 动画对象缓存，防止内存抖动
    std::unordered_map<ZombieState, cocos2d::Animation*> _animCache;

    float _attackRange;      // 近战攻击判定的触发距离
    float _moveSpeed;        // 常规行走速度
    float _runSpeed;         // 追击玩家时的速度
    cocos2d::Vec2 _velocity; // 物理速度向量

    ZombieState _state;      // 当前有限状态机状态
    MoveDirection _direction;// 朝向（左/右）

    // 内部工具：辅助创建动画序列
    cocos2d::Animation* createAnim(const std::string& name, int frameCount, float time);
    cocos2d::Animation* getAnimation(ZombieState state);

    /**
     * 创建攻击判定盒
     * 对于僵尸，这通常是在手部或身体前方创建的一个短距离 PhysicsShape
     */
    virtual void createAttackBox() override;

    float _aiTickTimer; // 控制 AI 扫描玩家频率的计时器

};

#endif // __ZOMBIE_H__