// Player.cpp
#include "Player.h"
#include "Weapon.h"
#include "Shield.h"
#include "Bow.h"
#include "Sword.h"
USING_NS_CC;
const float GRAVITY = 980.0f; 
Player* Player::createPlayer()
{
    Player* player = new (std::nothrow) Player();
    if (player && player->init())
    {
        player->autorelease();
        return player;
    }
    CC_SAFE_DELETE(player);
    return nullptr;
}
bool Player::init()
{
    if (!Sprite::initWithFile("Graph/Player/playerInit.png"))
        return false;

    _health = 100;
    _attack = 20;
    _defense = 10;
    _moveSpeed = 150.0f;
    _rollSpeed = 300.0f;
    _jumpSpeed = 400.0f;
    _state = ActionState::stand;
	_direction = MoveDirection::NONE;
    this->scheduleUpdate(); 
    playAnimation(ActionState::stand, true);
    return true;
}
void Player::update(float dt)
{
    Sprite::update(dt);

    cocos2d::Vec2 newPos = this->getPosition();

    if (_direction != MoveDirection::NONE &&
        (_state == ActionState::run || _state == ActionState::rollStart))
    {
        float speed = (_state == ActionState::rollStart) ? _rollSpeed : _moveSpeed;
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        newPos.x += speed * dt * dir;
        this->setFlippedX(dir == -1);
    }

    _velocity.y -= GRAVITY * dt;

    if (newPos.y <= 0)
    {
        newPos.y = 0;
        _velocity.y = 0;
    }

    this->setPosition(newPos + _velocity * dt);
}

void Player::changeState(ActionState newState)
{
    if (!canChangeTo(newState))
        return;

    if (_state == newState)
        return;

    this->stopActionByTag(1001);
    _state = newState;

    bool loop = StateTable[newState].loop;
    playAnimation(newState, loop);
}

bool Player::canChangeTo(ActionState newState)
{
    auto currentConfig = StateTable[_state];
    auto newConfig = StateTable[newState];
    if (!currentConfig.canBeInterrupted)
        return false;

    if (newConfig.priority < currentConfig.priority)
        return false;

    return true;
}
void Player::playAnimation(ActionState state, bool loop)
{
    Animation* anim = getAnimation(state);
    if (!anim) return;

    auto animate = Animate::create(anim);
    Action* action;
    if (loop)
        action = RepeatForever::create(animate);
    else
		action = animate;
    action->setTag(1001);
    this->runAction(action);
}
cocos2d::Animation* Player::createAnim(const std::string& name, int frameCount, float delay)
{
    auto anim = Animation::create();
    for (int i = 0; i < frameCount; ++i)
    {
        std::string frame = StringUtils::format("Graph/Player/%s_%02d-=-0-=-.png", name.c_str(), i);
        anim->addSpriteFrameWithFile(frame);
    }
    anim->setDelayPerUnit(delay);
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
        case ActionState::stand:       anim = createAnim("stand", 8, 0.1f); break;
        case ActionState::run:         anim = createAnim("run", 20, 0.05f); break;
        case ActionState::jumpUp:      anim = createAnim("jumpUp", 6, 0.08f); break;
        case ActionState::crouch:      anim = createAnim("crouch", 14, 0.05f); break;
        case ActionState::rollStart:   anim = createAnim("rollStart", 4, 0.06f); break;
        case ActionState::atkA:        anim = createAnim("atkA", 14, 0.05f); break;
        default:                        return nullptr;
    }
    anim->retain();
    _animationCache[state] = anim;
    return anim;
}
