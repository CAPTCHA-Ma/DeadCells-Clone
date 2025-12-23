#ifndef __PEOPLE_H__
#define __PEOPLE_H__
enum class MoveDirection
{
    LEFT,
    RIGHT
};
struct BasicAttributes
{
    int health;
    int attack;
    int defense;
};
enum PhysicsCategory
{
    NONE = 0,

    // --- 玩家相关---
    PLAYER_BODY = 1 << 0,  // 玩家物理身体（用于与地面、墙壁碰撞）
    PLAYER_ATTACK = 1 << 1,  // 玩家近战攻击判定框
    PLAYER_HURT = 1 << 2,  // 玩家受击框
    PLAYER_ARROW = 1 << 3,  // 玩家射出的箭

    // --- 敌人相关---
    ENEMY_BODY = 1 << 4,  // 敌人物理身体
    ENEMY_ATTACK = 1 << 5,  // 敌人近战攻击判定框
    ENEMY_HURT = 1 << 6,  // 敌人受击框
    ENEMY_ARROW = 1 << 7,  // 敌人箭矢
    ENEMY_BOMB = 1 << 8,  // 敌人炸弹

    GROUND = 1 << 9,  // 地面
    LADDER = 1 << 10, // 梯子
    PLATFORM = 1 << 11, // 平台
    AIR = 1 << 12, // 空气
    MIX = 1 << 13
};


#endif // __MOVE_DIRECTION_H__
