#include "Player.h"
#include "Weapon.h"
#include "Shield.h"
#include "Bow.h"
#include "Sword.h"
USING_NS_CC;

const float GRAVITY = 980.0f;


bool Player::init()
{
    if (!Sprite::initWithFile("Graph/Player/playerInit.png"))
        return false;

    // 属性初始化
    _health = 100;
    _moveSpeed = 150.0f;
    _rollSpeed = 300.0f;
    _jumpSpeed = 250.0f;
    _state = ActionState::stand;
    _direction = MoveDirection::RIGHT;

    auto body = PhysicsBody::createBox(this->getContentSize(), PhysicsMaterial(0.1f, 0.0f, 0.5f)); // 建议设置密度为 0.5f
    body->setDynamic(true);            // 动态体，受力影响
    body->setRotationEnable(false);    // 禁止旋转
    body->setGravityEnable(true);      // 启用重力
    this->setPhysicsBody(body);
    body->setVelocity(Vec2::ZERO);
    this->scheduleUpdate();
    this->_mainWeapon = new Sword(Sword::SwordType::AlucardSword);
    this->_subWeapon = NULL;
    playAnimation(ActionState::stand, true);
    return true;
}


void Player::changeState(ActionState newState)
{
    if (!canChangeTo(newState)) 
        return;
    if (_state == newState) 
        return;

    this->stopActionByTag(1001);

    switch (newState)
    {
        case ActionState::stand:this->stand();break;
        case ActionState::run:this->run(); break;
        case ActionState::jumpUp:this->jumpUp(); break;
        case ActionState::rollStart:this->rollStart();break;
		case ActionState::crouch:this->crouch(); break;
		case ActionState::atkA:this->atkA(); break;
		case ActionState::atkB:this->atkB(); break;
        case ActionState::AtkAlucardSwordA:this->AltackAlucardSwordA(); break;
		case ActionState::atkBackStabber:this->atkBackStabber(); break;
		case ActionState::AtkBaseballBatA:this->AtkBaseballBatA(); break;
		case ActionState::atkBroadSword:this->atkBroadSword(); break;
        default:break;
    }

    _state = newState;
    bool loop = StateTable[newState].loop;
    playAnimation(newState, loop);
}
void Player::atkWithWeapon(Weapon* weapon)
{
    if (!weapon)
    {
        changeState(ActionState::atkA);
        return;
    }
    Sword* sword = dynamic_cast<Sword*>(weapon);
    if (sword)
    {
        if (sword->getSwordType() == Sword::SwordType::AlucardSword)
            changeState(ActionState::AtkAlucardSwordA);
		else if (sword->getSwordType() == Sword::SwordType::BackStabber)
            changeState(ActionState::atkBackStabber);
        else if (sword->getSwordType() == Sword::SwordType::BaseballBat)
            changeState(ActionState::AtkBaseballBatA);
        else if (sword->getSwordType() == Sword::SwordType::BroadSword)
			changeState(ActionState::atkBroadSword);
        return;
    }
}
bool Player::canChangeTo(ActionState newState)
{
    if (newState == ActionState::stand)
        return true;

    auto currentConfig = StateTable[_state];
    auto newConfig = StateTable[newState];

    if (!currentConfig.canBeInterrupted)
        return false;
    if (newConfig.priority < currentConfig.priority)
        return false;

    return true;
}

void Player::stand()
{
    auto body = this->getPhysicsBody();
    if (body)
    {
        Vec2 currentVel = body->getVelocity();
        currentVel.x = 0;
        body->setVelocity(Vec2(0, currentVel.y));
    }
}
void Player::run()
{
    auto body = this->getPhysicsBody();
    if (body)
    {
        Vec2 currentVel = body->getVelocity();
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        currentVel.x = _moveSpeed * dir;
        body->setVelocity(currentVel);
        this->setFlippedX(dir == -1);
    }

}
void Player::hang()
{

}
void Player::jumpUp()
{
    auto body = this->getPhysicsBody();
    if (body)
    {
        Vec2 impulse(0, body->getMass() * _jumpSpeed);
        body->applyImpulse(impulse);
    }
}
void Player::rollStart()
{
    auto body = this->getPhysicsBody();
    if (body)
    {
        Vec2 currentVel = body->getVelocity();
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        currentVel.x = _rollSpeed * dir;
        body->setVelocity(currentVel);
	}
}
void Player::crouch()
{
    this->stand();
}
void Player::atkA()
{
    this->stand();
}
void Player::atkB()
{
    this->stand();
}
void Player::AltackAlucardSwordA()
{
    this->stand();
}
void Player::atkBackStabber()
{
    this->stand();
}
void Player::AtkBaseballBatA()
{
    this->stand();
}
void Player::atkBroadSword()
{
    this->stand();
}
void Player::playAnimation(ActionState state, bool loop)
{
    Animation* anim = getAnimation(state);
    if (!anim) return;

    auto animate = Animate::create(anim);
    Action* action = nullptr;

    if (loop)
    {
        action = RepeatForever::create(animate);
    }
    else
    {
        auto callback = CallFunc::create([this, state]() {
            if (_state == state)
            {
                changeState(ActionState::stand);
            }
            });

        action = Sequence::create(animate, callback, nullptr);
    }

    action->setTag(1001);
    this->runAction(action);
}

cocos2d::Animation* Player::createAnim(const std::string& name, int frameCount, float time)
{
    auto anim = Animation::create();
    for (int i = 0; i < frameCount; ++i)
    {
        std::string frame = StringUtils::format("Graph/Player/%s_%02d-=-0-=-.png", name.c_str(), i);
        anim->addSpriteFrameWithFile(frame);    
    }
    anim->setDelayPerUnit(time/frameCount);
    anim->setRestoreOriginalFrame(false);
    return anim;
}
cocos2d::Animation* Player::getAnimation(ActionState state)
{
    auto iter = _animationCache.find(state);
    if (iter != _animationCache.end())
        return iter->second;

    Animation* anim = nullptr;
    switch (state)
    {
        case ActionState::stand:     anim = createAnim("stand", 1, 1.0f); break;
        case ActionState::run:       anim = createAnim("run", 20, 1.0f); break;
        case ActionState::jumpUp:    anim = createAnim("jumpUp", 6, 1.0f); break;
        case ActionState::crouch:    anim = createAnim("crouch", 1, 0.2f); break;
        case ActionState::rollStart: anim = createAnim("rollStart", 4,0.5f); break;
        case ActionState::atkA:      anim = createAnim("atkA", 14, 0.5f); break;
		case ActionState::atkB:      anim = createAnim("atkB", 10, 0.5f); break;
		case ActionState::AtkAlucardSwordA: anim = createAnim("AtkAlucardSwordA", 20, 0.5f); break;
		case ActionState::atkBackStabber:  anim = createAnim("atkBackStabberA", 13, 0.5f); break;
		case ActionState::AtkBaseballBatA: anim = createAnim("AtkBaseballBatA", 10, 0.5f); break;
		case ActionState::atkBroadSword:   anim = createAnim("atkBroadSwordA", 15, 0.5f); break;
        default:                      return nullptr;
    }
    anim->retain();
    _animationCache[state] = anim;
    return anim;
}
