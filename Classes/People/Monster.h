// Monster.h
#ifndef __MONSTER_H__
#define __MONSTER_H__
#include "cocos2d.h"
#include "People.h"
#include "Arrow.h"
#include "Bomb.h"
enum class MonsterCategory
{
    Grenadier,
    Zombie,
    DeadArcher
};
class Monster : public cocos2d::Sprite
{
public:
    bool isDead() const { return _isDead; }
    bool isReadyToRemove() const { return _isReadyToRemove; }
    virtual bool init() = 0;
    virtual void idle()=0;     
	virtual void walk() = 0; 
    virtual void onDead() = 0;
    virtual void ai(float dt, cocos2d::Vec2 playerWorldPos) = 0;
    void struck(float attackPower);
	void dead();
    float getFinalAttack() { return _currentAttributes.attack; };
    static Monster* create(MonsterCategory category);
    MonsterCategory getMonsterType();

    const BasicAttributes& getAttributes() const { return _currentAttributes; }

    void clearHitTargets() { _hitTargets.clear(); }
    void recordHitTarget(cocos2d::Node* target) { _hitTargets.insert(target); }
    bool hasHitTarget(cocos2d::Node* target) const {
        return _hitTargets.find(target) != _hitTargets.end();
    }

    void update(float dt);

    //金币奖励
    using DeathCallback = std::function<void(int)>;
    void setDeathCallback(DeathCallback cb) { _deathCallback = cb; }

protected:
    bool _goldAwarded = false; // 新增：金币是否已发放的标记

public:
    bool isGoldAwarded() const { return _goldAwarded; }
    void setGoldAwarded(bool awarded) { _goldAwarded = awarded; }
protected:
    DeathCallback _deathCallback = nullptr;


    std::set<cocos2d::Node*> _hitTargets;
    void setupHPBar();
    void updateHPBar();
    cocos2d::DrawNode* _hpBarNode = nullptr;
    CC_SYNTHESIZE(BasicAttributes, _currentAttributes, CurrentAttributes);
    CC_SYNTHESIZE(float, _maxHealth, MaxHealth);
    MonsterCategory _type;
    virtual bool isAttackState() = 0;
    bool _isDead = false;
    bool _isReadyToRemove = false; // 新增：标记动画播放完毕，可以从容器删除

    cocos2d::Sprite* _sprite = nullptr;
    cocos2d::PhysicsBody* _body = nullptr;

    cocos2d::Node* _attackNode = nullptr;
   
    virtual void createAttackBox()=0;
    void removeAttackBox();
    void createBody(const cocos2d::Size& size, const cocos2d::Vec2& offset);

};


#endif // __MONSTER_H__