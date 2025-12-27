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

cocos2d::Vec2 PlayerLayer::GetVelo()
{
    
	auto body = _player->getPhysicsBody();
	return body->getVelocity();

}

void PlayerLayer::setupEventListeners()
{
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event)
        {
            if (_player->isLethalState())
                return;
            auto body = _player->getPhysicsBody();
            if (!body)
                return;
            switch (keyCode)
            {
                case EventKeyboard::KeyCode::KEY_A:
                    _leftPressed = true;
                    _player->changeDirection(MoveDirection::LEFT);
                    if (_player->_state != ActionState::jumpUp && _player->_state != ActionState::jumpDown)
                        _player->changeState(ActionState::run);
                    break;

                case EventKeyboard::KeyCode::KEY_D:
                    _rightPressed = true;
                    _player->changeDirection(MoveDirection::RIGHT);
                    if (_player->_state != ActionState::jumpUp && _player->_state != ActionState::jumpDown)
                        _player->changeState(ActionState::run);
                    break;
                case EventKeyboard::KeyCode::KEY_W://ÅÊÅÀ
					_player->_directionY = UpDownDirection::UP;
                    if (_isBelowLadder)
                    {

                        _player->changeState(ActionState::hanging);
                        _player->changeState(ActionState::climbing);

                    }
					break;
                case EventKeyboard::KeyCode::KEY_SPACE://ÌøÔ¾

                    if (_player->_state == ActionState::climbing || _player->_state == ActionState::hanging)
                    {
                        break;
					}

                    if (_isPassingPlatform)
                    {

                        _player->changeState(ActionState::climbedge);

                    }

                    if (_downPressed)
                    {

                        if (_isDropping) break;
                        _isDropping = true;

                        this->scheduleOnce([this](float dt) {
                            _isDropping = false;
                            }, 0.3f, "reset_drop_flag");

                    }
                    else _player->changeState(ActionState::jumpUp);
                    break;
                case EventKeyboard::KeyCode::KEY_J://Ö÷ÎäÆ÷¹¥»÷
                    _player->whenOnAttackKey(_player->_mainWeapon);
                    break;
                case EventKeyboard::KeyCode::KEY_K://¸±ÎäÆ÷¹¥»÷
                    _player->whenOnAttackKey(_player->_subWeapon);
                    break;
                case EventKeyboard::KeyCode::KEY_S://ÏÂ¶×

                    _player->_directionY = UpDownDirection::DOWN;
                    _downPressed = true;
                    if (_isAboveLadder || _player->_state == ActionState::hanging)
                    {

						CCLOG("change to climbing\n");

                        _player->changeState(ActionState::hanging);
                        _player->changeState(ActionState::climbing);
                        break;

                    }

                    _player->changeState(ActionState::crouch);
                    break;
                case EventKeyboard::KeyCode::KEY_L://¹ö¶¯
                    _player->changeState(ActionState::rollStart);
                    break;
                case EventKeyboard::KeyCode::KEY_Q://½»»»Ö÷¸±ÎäÆ÷
                    _player->swapWeapon();
                    break;
                case EventKeyboard::KeyCode::KEY_E: // Ê°È¡ÎäÆ÷
                    if (_nearbyWeapon)
                        this->getNewWeapon();
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
				_player->_directionY = UpDownDirection::NONE;
                _downPressed = false;
                if (_player->_state == ActionState::crouch)
                {
                    _player->changeState(ActionState::idle);
                }

                if (_player->_state == ActionState::climbing)
                {

					_player->changeState(ActionState::hanging);
                    
				}

            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_W)
            {

                _player->_directionY = UpDownDirection::NONE;
                if (_player->_state == ActionState::climbing)
                {

                    _player->changeState(ActionState::hanging);

                }

            }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
cocos2d::Vec2 PlayerLayer::getPlayerWorldPosition() const
{
    if (!_player) 
        return Vec2::ZERO;
    return this->convertToWorldSpace(_player->getPosition());
}
void PlayerLayer::struck(float attackPower, cocos2d::Vec2 sourcePos)
{
    float diffX = sourcePos.x - getPlayerWorldPosition().x;

    if (diffX > 0)
    {
        _player->changeDirection(MoveDirection::RIGHT);
    }
    else 
    {
        _player->changeDirection(MoveDirection::LEFT);
    }
    _player->struck(attackPower);
}
void PlayerLayer::getNewWeapon()
{
    if (!_nearbyWeapon || !_nearbyWeapon->getParent() || !_player)
        return;
    Vec2 dropPos = _nearbyWeapon->getPosition();
    WeaponNode* weaponNodeToRemove = _nearbyWeapon;
    Weapon* newWpData = weaponNodeToRemove->pickUp();


    _nearbyWeapon = nullptr;
    Weapon* oldWpData = _player->getMainWeapon();
    _player->getNewWeapon(newWpData);
    if (oldWpData)
    {
        auto droppedNode = WeaponNode::create(oldWpData, dropPos);
        if (this->getParent())
        {
            this->getParent()->addChild(droppedNode);
            float xOffset = (_player->_direction == MoveDirection::RIGHT) ? -40.0f : 40.0f;
            auto jump = JumpBy::create(0.4f, Vec2(xOffset, -10), 30, 1);
            droppedNode->runAction(jump);
        }
    }
}
void PlayerLayer::update(float dt)
{
    auto body = _player->getPhysicsBody();
    if (_player->isLethalState())
    {
        _player->update(dt);
        return;
    }

    if (_player->_state == ActionState::climbing || _player->_state == ActionState::hanging)
    {

        if (body->getVelocity().y < 0.1f && _isContactBottom)
        {

            body->setVelocity(Vec2(0, 0));
            body->setGravityEnable(true);
			_player->changeState(ActionState::idle);

		}

        if (body->getVelocity().y > 0.1f && !(_isAboveLadder || _isBelowLadder))
        {

            body->setGravityEnable(true);
            _player->changeState(ActionState::idle);

        }

    }

    if (_player->_state == ActionState::climbedge && !_isPassingPlatform)
    {

		body->setGravityEnable(true);
        body->setVelocity(Vec2::ZERO);
        _player->changeState(ActionState::idle);

    }

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