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

    PLAYER_BODY = 1 << 0, // 玩家物理身体
    PLAYER_ATTACK = 1 << 1, // 玩家攻击框
    PLAYER_HURT = 1 << 2, // 玩家受击框

    ENEMY_BODY = 1 << 3, // 敌人物理身体
    ENEMY_ATTACK = 1 << 4, // 敌人攻击框
    ENEMY_HURT = 1 << 5, // 敌人受击框

    GROUND = 1 << 6  // 地面
};


#endif // __MOVE_DIRECTION_H__
