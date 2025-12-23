#include "PlayerLayer.h"
USING_NS_CC;
PlayerLayer* PlayerLayer::create(cocos2d::Vec2 pos)
{
    PlayerLayer* pRet = new(std::nothrow) PlayerLayer();
    if (pRet && pRet->init(pos))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}
bool PlayerLayer::init(cocos2d::Vec2 pos)
{
    if (!Layer::init())
        return false;

    this->setName("PlayerLayer"); 
    _player = Player::create();
    _player->setPosition(pos);
	_player->setName("Player");
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
					_rightPressed = false;
                    _player->changeDirection(MoveDirection::LEFT);
                    _player->changeState(ActionState::run);
                    break;
                case EventKeyboard::KeyCode::KEY_D://ÓÒÒÆ
                    _rightPressed = true;
					_leftPressed = false;
                    _player->changeDirection(MoveDirection::RIGHT);
                    _player->changeState(ActionState::run);
                    break;
                case EventKeyboard::KeyCode::KEY_SPACE://ÌøÔ¾
                    _player->changeState(ActionState::jumpUp);
                    break;
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
                    _player->changeState(ActionState::idle);
                }
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_D)
            {
                _rightPressed = false;
                if (_player->_state == ActionState::run)
                {
                    _player->changeState(ActionState::idle);
                }
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_S)
            {
                if (_player->_state == ActionState::crouch)
                {
                    _player->changeState(ActionState::idle);
                }
            }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
void PlayerLayer::update(float dt)
{
    auto body = _player->getPhysicsBody();
    if (_leftPressed && !_rightPressed)
    {
        _player->changeDirection(MoveDirection::LEFT);
        if (_player->_state == ActionState::jumpDown || _player->_state == ActionState::jumpUp)
        {
            if (!body)
                return;
			_player->giveVelocityX(_player->_runSpeed);
        }
            _player->changeState(ActionState::run);
    }
    else if (_rightPressed && !_leftPressed)
    {
        _player->_direction = MoveDirection::RIGHT;
        if (_player->_state == ActionState::jumpDown || _player->_state == ActionState::jumpUp)
        {
            if (!body)
                return;
            _player->giveVelocityX(_player->_runSpeed);
        }
        _player->changeState(ActionState::run);
    }
    else
    {
        if (_player->_state == ActionState::jumpDown || _player->_state == ActionState::jumpUp)
        {
            _player->set0VelocityX();
        }
    }
    _player->update(dt);
}


