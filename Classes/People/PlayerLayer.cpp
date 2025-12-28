#include "PlayerLayer.h"

USING_NS_CC;

PlayerLayer* PlayerLayer::create(Vec2 pos) {
    PlayerLayer* pRet = new(std::nothrow) PlayerLayer();
    if (pRet && pRet->init(pos)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool PlayerLayer::init(Vec2 pos) {
    if (!Layer::init()) return false;

    this->setName("PlayerLayer");

    _player = Player::create();
    _player->setPosition(pos);
    _player->setName("Player");
    this->addChild(_player);

    this->setupEventListeners();
    this->scheduleUpdate();

    return true;
}

// --- 战斗判定辅助 ---
void PlayerLayer::recordMonsterHit(Monster* monster) {
    if (monster) _hitMonsters.insert(monster);
}

bool PlayerLayer::isMonsterAlreadyHit(Monster* monster) {
    return _hitMonsters.find(monster) != _hitMonsters.end();
}

void PlayerLayer::clearHitMonsters() {
    _hitMonsters.clear();
}

// --- 基础属性接口 ---
Vec2 PlayerLayer::getPlayerWorldPosition() const {
    if (!_player) return Vec2::ZERO;
    return this->convertToWorldSpace(_player->getPosition());
}

Vec2 PlayerLayer::GetVelo() {
    auto body = _player->getPhysicsBody();
    return body ? body->getVelocity() : Vec2::ZERO;
}

bool PlayerLayer::spendGold(int amount) {
    if (_gold >= amount) {
        _gold -= amount;
        return true;
    }
    return false;
}

void PlayerLayer::healthUp()
{

    auto attrs = _player->getCurrentAttributes();
    attrs.health = _player->getMaxHealth();
    _player->setCurrentAttributes(attrs);
    _player->updateHPBar();

}

// --- 键盘事件 ---
void PlayerLayer::setupEventListeners() {
    auto listener = EventListenerKeyboard::create();

    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        if (_player->isLethalState()) return;
        auto body = _player->getPhysicsBody();
        if (!body) return;

        switch (keyCode) {
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

            case EventKeyboard::KeyCode::KEY_W:
                _player->_directionY = UpDownDirection::UP;
                if (_isBelowLadder) {
                    _player->changeState(ActionState::hanging);
                    _player->changeState(ActionState::climbing);
                }
                break;

            case EventKeyboard::KeyCode::KEY_SPACE:
                if (_player->_state == ActionState::climbing || _player->_state == ActionState::hanging) break;
                if (_isPassingPlatform) _player->changeState(ActionState::climbedge);

                if (_downPressed) {
                    if (_isDropping) break;
                    _isDropping = true;
                    this->scheduleOnce([this](float dt) { _isDropping = false; }, 0.3f, "reset_drop_flag");
                } else {
                    _player->changeState(ActionState::jumpUp);
                }
                break;

            case EventKeyboard::KeyCode::KEY_J:
                this->clearHitMonsters(); // 开始攻击，清空名单
                _player->whenOnAttackKey(_player->_mainWeapon);
                break;

            case EventKeyboard::KeyCode::KEY_K:
                this->clearHitMonsters(); // 开始攻击，清空名单
                _player->whenOnAttackKey(_player->_subWeapon);
                break;

            case EventKeyboard::KeyCode::KEY_S:
                _player->_directionY = UpDownDirection::DOWN;
                _downPressed = true;
                if (_isAboveLadder || _player->_state == ActionState::hanging) {
                    _player->changeState(ActionState::hanging);
                    _player->changeState(ActionState::climbing);
                } else {
                    _player->changeState(ActionState::crouch);
                }
                break;

            case EventKeyboard::KeyCode::KEY_L:
                _player->changeState(ActionState::rollStart);
                break;

            case EventKeyboard::KeyCode::KEY_Q:
                _player->swapWeapon();
                break;

            case EventKeyboard::KeyCode::KEY_E:
                if (_nearbyWeapon)
                {
                    int cost = _nearbyWeapon->getPrice();

                    if (cost == 0)
                    {
                        this->getNewWeapon();
                    }
                    else
                    {
                        if (this->_gold >= cost)
                        {
                            this->_gold -= cost;
                            this->getNewWeapon();
                        }
                        else
                        {

                            if (_nearbyWeapon->getChildByName("NoMoneyTip"))
                            {
                                _nearbyWeapon->removeChildByName("NoMoneyTip");
                            }

                            auto tipLabel = Label::createWithTTF("Not Enough Gold!", "fonts/fusion-pixel.ttf", 18);

                            tipLabel->setColor(Color3B::RED);
                            tipLabel->enableOutline(Color4B::WHITE, 1);
                            tipLabel->setName("NoMoneyTip");

                            tipLabel->setPosition(Vec2(_nearbyWeapon->getContentSize().width / 2,
                                _nearbyWeapon->getContentSize().height + 60));

                            tipLabel->setGlobalZOrder(9999);

                            _nearbyWeapon->addChild(tipLabel);

                            float duration = 1.0f;
                            auto moveUp = MoveBy::create(duration, Vec2(0, 30));
                            auto fadeOut = FadeOut::create(duration);
                            auto spawn = Spawn::create(moveUp, fadeOut, nullptr);
                            auto remove = RemoveSelf::create();

                            tipLabel->runAction(Sequence::create(spawn, remove, nullptr));
                        }
                    }

                }
                break;
            default:
                break;
        }
    };

    listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_A) {
            _leftPressed = false;
            if (_player->_state == ActionState::run) _player->changeState(ActionState::idle);
        } else if (keyCode == EventKeyboard::KeyCode::KEY_D) {
            _rightPressed = false;
            if (_player->_state == ActionState::run) _player->changeState(ActionState::idle);
        } else if (keyCode == EventKeyboard::KeyCode::KEY_S) {
            _player->_directionY = UpDownDirection::NONE;
            _downPressed = false;
            if (_player->_state == ActionState::crouch) _player->changeState(ActionState::idle);
            if (_player->_state == ActionState::climbing) _player->changeState(ActionState::hanging);
        } else if (keyCode == EventKeyboard::KeyCode::KEY_W) {
            _player->_directionY = UpDownDirection::NONE;
            if (_player->_state == ActionState::climbing) _player->changeState(ActionState::hanging);
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// --- 逻辑处理 ---
void PlayerLayer::struck(float attackPower, Vec2 sourcePos) {
    float diffX = sourcePos.x - getPlayerWorldPosition().x;
    _player->changeDirection(diffX > 0 ? MoveDirection::RIGHT : MoveDirection::LEFT);
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

void PlayerLayer::update(float dt) {
    auto body = _player->getPhysicsBody();
    if (_player->isLethalState()) {
        _player->update(dt);
        return;
    }

    // 梯子逻辑
    if (_player->_state == ActionState::climbing || _player->_state == ActionState::hanging) {
        if (body->getVelocity().y < 0.1f && _isContactBottom) {
            body->setVelocity(Vec2::ZERO);
            body->setGravityEnable(true);
            _player->changeState(ActionState::idle);
        }
        if (body->getVelocity().y > 0.1f && !(_isAboveLadder || _isBelowLadder)) {
            body->setGravityEnable(true);
            _player->changeState(ActionState::idle);
        }
    }

    // 边缘攀爬逻辑
    if (_player->_state == ActionState::climbedge && !_isPassingPlatform) {
        body->setGravityEnable(true);
        body->setVelocity(Vec2::ZERO);
        _player->changeState(ActionState::idle);
    }

    // 移动更新
    if (_leftPressed && !_rightPressed) {
        _player->changeDirection(MoveDirection::LEFT);
        if (_player->_state == ActionState::jumpDown || _player->_state == ActionState::jumpUp) {
            _player->giveVelocityX(_player->_runSpeed);
        }
        _player->changeState(ActionState::run);
    } else if (_rightPressed && !_leftPressed) {
        _player->changeDirection(MoveDirection::RIGHT);
        if (_player->_state == ActionState::jumpDown || _player->_state == ActionState::jumpUp) {
            _player->giveVelocityX(_player->_runSpeed);
        }
        _player->changeState(ActionState::run);
    } else {
        if (_player->_state == ActionState::jumpDown || _player->_state == ActionState::jumpUp) {
            _player->set0VelocityX();
        }
    }

    _player->update(dt);
}