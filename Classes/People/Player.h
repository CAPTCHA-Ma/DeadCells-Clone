// Player.h
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include "Weapon.h"

class Player : public cocos2d::Sprite
{
public:
    static Player* createPlayer();
    virtual bool init() override;

    // 属性
    CC_SYNTHESIZE(int, _health, Health);
    CC_SYNTHESIZE(int, _attack, Attack);
    CC_SYNTHESIZE(int, _defense, Defense);
    CC_SYNTHESIZE(float, _moveSpeed, MoveSpeed);

    // 武器系统
    void equipWeapon(Weapon* weapon);
    Weapon* getEquippedWeapon() const 
    { 
        return _currentWeapon;
    }
    int getFinalAttackPower() const;
    int getFinalDefensePower() const;

    // 行为
    void moveTowards(const cocos2d::Vec2& targetPosition);//朝着目标坐标移动
    void moveRight();//水平移动
    void moveLeft();//水平移动
    void jump();//跳跃
    void crouch();//下蹲
    void roll();//翻滚
    void attack();    // 攻击
    void defend();    // 防御
    void moveFromKeyBoard(char input);//键盘控制移动
private:
    Player();
    virtual ~Player();

	Weapon* _mainWeapon;// 主武器
	Weapon* _subWeapon;// 副武器
	Weapon* _currentWeapon;// 当前装备的武器
	cocos2d::Node* _handNode; // 用于挂载武器的节点
};


#endif // __PLAYER_H__