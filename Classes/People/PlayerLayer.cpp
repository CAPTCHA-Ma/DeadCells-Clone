#include "PlayerLayer.h"
USING_NS_CC;
bool PlayerLayer::init()
{
    if (!Layer::init())
        return false;


    _player = Player::create();
    _player->setPosition(Vec2(400, 200));
    this->addChild(_player);

    this->setupEventListeners();
    this->scheduleUpdate();

    return true;
}
void PlayerLayer::setupEventListeners()
{
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event)
        {
            auto body = _player->getPhysicsBody();
            if (!body)
                return;
            switch (keyCode)
            {
                case EventKeyboard::KeyCode::KEY_A://×óÒÆ
                    _leftPressed = true;
                    _player->_direction = MoveDirection::LEFT;
                    _player->changeState(ActionState::run);
                    break;
                case EventKeyboard::KeyCode::KEY_D://ÓÒÒÆ
                    _rightPressed = true;
                    _player->_direction = MoveDirection::RIGHT;
                    _player->changeState(ActionState::run);
                    break;
                case EventKeyboard::KeyCode::KEY_SPACE://ÌøÔ¾
                {
                    float current_vy = body->getVelocity().y;
                    if (std::abs(current_vy) < 0.1f)
                    {
                        float mass = body->getMass();
                        Vec2 impulse(0, mass * _player->_jumpSpeed);
                        body->applyImpulse(impulse);

                        _player->changeState(ActionState::jumpUp);
                    }
                    break;
                }

                case EventKeyboard::KeyCode::KEY_J://Ö÷ÎäÆ÷¹¥»÷
                    _player->whenOnAttackKey(_player->_mainWeapon);
                    break;
                case EventKeyboard::KeyCode::KEY_K://¸±ÎäÆ÷¹¥»÷
                    _player->whenOnAttackKey(_player->_subWeapon);
                    break;
                case EventKeyboard::KeyCode::KEY_S://ÏÂ¶×
                    _player->changeState(ActionState::crouch);
                    break;
                case EventKeyboard::KeyCode::KEY_L://¹ö¶¯
                    _player->changeState(ActionState::rollStart);
                    break;
                default:
                    break;
            }
        };

    listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event)
        {

            if (keyCode == EventKeyboard::KeyCode::KEY_A)
            {
                _leftPressed = false;
                if (_player->_state == ActionState::run)
                {
                    _player->changeState(ActionState::stand);
                }
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_D)
            {
                _rightPressed = false;
                if (_player->_state == ActionState::run)
                {
                    _player->changeState(ActionState::stand);
                }
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_S)
            {
                if (_player->_state == ActionState::crouch)
                {
                    _player->changeState(ActionState::stand);
                }
            }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
void PlayerLayer::update(float dt)
{
    auto body = _player->getPhysicsBody();
    if (!body)
        return;


    if (_leftPressed && !_rightPressed)
    {
        _player->_direction = MoveDirection::LEFT;
        _player->changeState(ActionState::run);
    }
    else if (_rightPressed && !_leftPressed)
    {
        _player->_direction = MoveDirection::RIGHT;
        _player->changeState(ActionState::run);
    }
    else
    {
        if (_player->_state == ActionState::run)
            _player->changeState(ActionState::stand);
    }
}


