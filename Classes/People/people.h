#ifndef __PEOPLE_H__
#define __PEOPLE_H__
enum class MoveDirection
{
    LEFT,
    RIGHT

};

enum class UpDownDirection
{

    NONE,
    UP,
    DOWN

};

struct BasicAttributes
{
    float health;//生命值
    float attack;//攻击力
    float defense;//防御力

    BasicAttributes() = default;
    BasicAttributes(float h, float a, float d) noexcept : health(h), attack(a), defense(d) {}

    // 显式声明拷贝构造函数
    BasicAttributes(const BasicAttributes& other) noexcept
        : health(other.health), attack(other.attack), defense(other.defense) 
    {}

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
};
enum PhysicsCategory
{
    NONE = 0,


    PLAYER_BODY = 1 << 0,  // 玩家物理身体
    PLAYER_ATTACK = 1 << 1,  // 玩家攻击判定框
    PLAYER_ARROW = 1 << 3,  // 玩家射出的箭
    WEAPON = 1 << 14,

    ENEMY_BODY = 1 << 4,  // 敌人物理身体
    ENEMY_ATTACK = 1 << 5,  // 敌人攻击判定框
    ENEMY_ARROW = 1 << 7,  // 敌人箭矢
    ENEMY_BOMB = 1 << 8,  // 敌人炸弹

    GROUND = 1 << 9,  // 地面
    LADDER = 1 << 10, // 梯子
    PLATFORM = 1 << 11, // 平台
    AIR = 1 << 12, // 空气
    MIX = 1 << 13
};



#endif // __PEOPLE_H__