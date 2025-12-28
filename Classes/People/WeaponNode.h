#ifndef __WEAPONNODE_H__
#define __WEAPONNODE_H__

#include "cocos2d.h"
#include "Weapon.h"
#include "Sword.h"
#include "Bow.h"
#include "Shield.h"
#include "people.h"
#include <string>
#include <map>

/**
 * WeaponNode 类：武器在游戏场景中的物理表现
 * 继承自 cocos2d::Sprite，使其具有位置、贴图和物理属性
 */
class WeaponNode : public cocos2d::Sprite {
public:
    // --- 静态工厂方法 ---

    // 通用创建方法：传入已有的武器数据
    static WeaponNode* create(Weapon* weaponData, cocos2d::Vec2 pos);

    // 快捷创建方法：直接通过子类型在指定位置生成对应武器节点
    static WeaponNode* createSword(Sword::SwordType type, cocos2d::Vec2 pos);
    static WeaponNode* createBow(Bow::BowType type, cocos2d::Vec2 pos);
    static WeaponNode* createShield(Shield::ShieldType type, cocos2d::Vec2 pos);

    /**
     * 初始化函数
     * 负责将逻辑数据 _weapon 与视觉资源（贴图）、物理身体绑定
     */
    bool init(Weapon* weaponData, cocos2d::Vec2 pos);

    /**
     * 捡起动作
     * 当玩家交互时调用。该函数会断开逻辑对象与显示节点的联系，
     * 返回 Weapon 指针供玩家持有，并销毁 WeaponNode 自身。
     */
    Weapon* pickUp();

    // 获取内部武器数据的指针
    Weapon* getWeapon() const { return _weapon; }

    // 析构函数：由于 _weapon 是手动 new 出来的，需要在这里负责释放内存
    ~WeaponNode();

    // --- 商店/交易系统属性 ---
    void setPrice(int price);
    int getPrice() const { return _price; }

private:
    Weapon* _weapon;                    // 核心逻辑数据对象（所有权在此，直到被捡起）
    int _price = 0;                     // 购买此武器所需金币（0表示免费掉落物）
    cocos2d::Label* _priceLabel = nullptr; // 显示在武器上方的价格文字标签

};

#endif // __WEAPONNODE_H__