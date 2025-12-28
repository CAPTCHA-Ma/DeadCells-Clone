#ifndef __PEOPLE_H__
#define __PEOPLE_H__

/**
 * @brief 水平移动方向
 */
enum class MoveDirection
{
    LEFT,   // 向左
    RIGHT   // 向右
};

/**
 * @brief 垂直移动方向
 * * 主要用于梯子攀爬或特殊地形交互
 */
enum class UpDownDirection
{
    NONE,   // 无方向
    UP,     // 向上
    DOWN    // 向下
};

/**
 * @brief 基础属性结构体
 * * 采用数值管理角色的生命值、攻击力和防御力，并重载了拷贝与赋值逻辑以保证安全。
 */
struct BasicAttributes
{
    float health;  // 当前生命值
    float attack;  // 基础攻击力
    float defense; // 防御力（通常按百分比或减伤公式计算）

    // 默认构造函数
    BasicAttributes() = default;

    // 参数构造函数
    BasicAttributes(float h, float a, float d) noexcept : health(h), attack(a), defense(d) {}

    // 显式拷贝构造函数
    BasicAttributes(const BasicAttributes& other) noexcept
        : health(other.health), attack(other.attack), defense(other.defense)
    {
    }

    // 拷贝赋值运算符
    BasicAttributes& operator=(const BasicAttributes& other) noexcept
    {
        if (this != &other)
        {
            health = other.health;
            attack = other.attack;
            defense = other.defense;
        }
        return *this;
    }

    // 移动赋值运算符
    BasicAttributes& operator=(BasicAttributes&& other) noexcept = default;

    // 乘法运算符重载：用于处理属性加成（如吃掉翻倍道具）
    BasicAttributes operator*(int value) const
    {
        return BasicAttributes(health * value, attack * value, defense * value);
    }
};

/**
 * @brief 物理碰撞掩码 (Bitmask)
 * * 使用位运算定义不同的物理层级。通过位与(&)和位或(|)来决定哪些物体可以相互碰撞或触发事件。
 */
enum PhysicsCategory
{
    NONE = 0,

    // 玩家相关层 (1-8位)
    PLAYER_BODY = 1 << 0,  // 玩家实体（受重力影响的躯干）
    PLAYER_ATTACK = 1 << 1,  // 玩家近战攻击判定框（临时生成）
    PLAYER_ARROW = 1 << 3,  // 玩家射出的远程道具

    // 敌人相关层 (5-9位)
    ENEMY_BODY = 1 << 4,  // 敌人实体
    ENEMY_ATTACK = 1 << 5,  // 敌人近战判定框
    ENEMY_ARROW = 1 << 7,  // 敌人箭矢
    ENEMY_BOMB = 1 << 8,  // 敌人炸弹等投掷物

    // 环境与地形层 (10-14位)
    GROUND = 1 << 9,  // 坚实地面
    LADDER = 1 << 10, // 梯子（允许穿过，但触发攀爬逻辑）
    PLATFORM = 1 << 11, // 单向平台（允许从下方跳上）
    AIR = 1 << 12, // 空气区域
    MIX = 1 << 13, // 混合地形

    // 交互与装备 (15位以后)
    WEAPON = 1 << 14, // 掉落在地上的武器掉落物
    INTERACTABLE = 1 << 15, // 可交互物品（宝箱、开关等）
    SHIELD = 1 << 16  // 盾牌格挡层
};

#endif // __PEOPLE_H__