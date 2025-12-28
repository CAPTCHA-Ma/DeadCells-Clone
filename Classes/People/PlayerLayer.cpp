#include "PlayerLayer.h"

USING_NS_CC;

/**
 * 静态创建函数。
 * @param pos 玩家初始生成的坐标。
 */
PlayerLayer* PlayerLayer::create(Vec2 pos) {
    PlayerLayer* pRet = new(std::nothrow) PlayerLayer();
    if (pRet && pRet->init(pos)) {
        pRet->autorelease(); // 加入自动释放池
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

/**
 * 初始化层，设置玩家实例和键盘监听。
 */
bool PlayerLayer::init(Vec2 pos) {
    if (!Layer::init()) return false;

    this->setName("PlayerLayer");

    // 创建并配置玩家
    _player = Player::create();
    _player->setPosition(pos);
    _player->setName("Player");
    this->addChild(_player);

    this->setupEventListeners(); // 绑定输入
    this->scheduleUpdate();      // 开启每帧更新

    return true;
}

// -----------------------------------------------------------------------------
// 经济系统逻辑
// -----------------------------------------------------------------------------

/**
 * 增加金币并同步 UI。
 */
void PlayerLayer::addGold(int amount) {
    _gold += amount;
    if (_player) {
        _player->updateGoldDisplay(_gold); // 更新玩家头顶的漂浮文字
    }
}

/**
 * 扣除金币，返回扣费是否成功（余额判定）。
 */
bool PlayerLayer::reduceGold(int amount) {
    if (_gold >= amount) {
        _gold -= amount;
        if (_player) {
            _player->updateGoldDisplay(_gold);
        }
        return true;
    }
    return false; // 金币不足
}

// -----------------------------------------------------------------------------
// 战斗判定辅助 (防止多段伤害重复触发)
// -----------------------------------------------------------------------------

/**
 * 记录在本次攻击动作中已被击中的怪物。
 */
void PlayerLayer::recordMonsterHit(Monster* monster) {
    if (monster) _hitMonsters.insert(monster);
}

/**
 * 检查怪物是否在本次攻击中已受过伤。
 */
bool PlayerLayer::isMonsterAlreadyHit(Monster* monster) {
    return _hitMonsters.find(monster) != _hitMonsters.end();
}

/**
 * 清空攻击名单（通常在每次按下攻击键时调用）。
 */
void PlayerLayer::clearHitMonsters() {
    _hitMonsters.clear();
}

// -----------------------------------------------------------------------------
// 基础属性接口
// -----------------------------------------------------------------------------

/**
 * 获取玩家在世界坐标系下的位置（用于 UI 跟随或摄像机逻辑）。
 */
Vec2 PlayerLayer::getPlayerWorldPosition() const {
    if (!_player) return Vec2::ZERO;
    return this->convertToWorldSpace(_player->getPosition());
}

/**
 * 获取玩家当前的物理初速度。
 */
Vec2 PlayerLayer::GetVelo() {
    auto body = _player->getPhysicsBody();
    return body ? body->getVelocity() : Vec2::ZERO;
}

/**
 * 治疗逻辑。
 * @param value 恢复的生命值量，不能超过最大生命值。
 */
void PlayerLayer::healthUp(int value)
{
    auto attrs = _player->getCurrentAttributes();
    int newHealth = attrs.health + value;
    int maxHealth = _player->getMaxHealth();

    if (newHealth > maxHealth) newHealth = maxHealth;

    attrs.health = newHealth;
    _player->setCurrentAttributes(attrs);
    _player->updateHPBar(); // 刷新血条 UI
}

// -----------------------------------------------------------------------------
// 键盘事件处理 (核心逻辑控制)
// -----------------------------------------------------------------------------

void PlayerLayer::setupEventListeners() {
    auto listener = EventListenerKeyboard::create();

    // 按下按键逻辑
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        if (_player->isLethalState()) return; // 死亡或致命受击中，忽略输入
        auto body = _player->getPhysicsBody();
        if (!body) return;

        switch (keyCode) {
            case EventKeyboard::KeyCode::KEY_A: // 向左跑
                _leftPressed = true;
                _player->changeDirection(MoveDirection::LEFT);
                if (_player->_state != ActionState::jumpUp && _player->_state != ActionState::jumpDown)
                    _player->changeState(ActionState::run);
                break;

            case EventKeyboard::KeyCode::KEY_D: // 向右跑
                _rightPressed = true;
                _player->changeDirection(MoveDirection::RIGHT);
                if (_player->_state != ActionState::jumpUp && _player->_state != ActionState::jumpDown)
                    _player->changeState(ActionState::run);
                break;

            case EventKeyboard::KeyCode::KEY_W: // 向上爬梯子
                _player->_directionY = UpDownDirection::UP;
                if (_isBelowLadder) {
                    _player->changeState(ActionState::hanging);
                    _player->changeState(ActionState::climbing);
                }
                break;

            case EventKeyboard::KeyCode::KEY_SPACE: // 跳跃逻辑
                // 情况 1: 在梯子上跳跃（脱离梯子）
                if (_player->_state == ActionState::climbing || _player->_state == ActionState::hanging)
                {
                    auto body = _player->getPhysicsBody();
                    if (body) {
                        body->setGravityEnable(true);
                        body->setVelocity(Vec2(body->getVelocity().x, 350.0f));
                        _player->changeState(ActionState::jumpUp);
                        _jumpCount = 1;
                    }
                    break;
                }
                // 情况 2: 平台边缘攀爬跳跃
                if (_isPassingPlatform) {
                    _player->changeState(ActionState::climbedge);
                    break;
                }
                // 情况 3: 普通地面跳跃及二段跳
                if (_downPressed) {
                    // S+Space 下跳平台逻辑（此处仅设置标志）
                    if (_isDropping) break;
                    _isDropping = true;
                    this->scheduleOnce([this](float dt) { _isDropping = false; }, 0.3f, "reset_drop_flag");
                }
                else {
                    bool canJump = false;
                    if (_player->_state == ActionState::idle || _player->_state == ActionState::run) {
                        canJump = true;
                        _jumpCount = 1;
                    }
                    else if (_jumpCount < _maxJumpCount && _jumpCount > 0) {
                        canJump = true;
                        _jumpCount++;
                    }
                    if (canJump) {
                        body->setVelocity(Vec2(body->getVelocity().x, 450.0f));
                        _player->changeState(ActionState::jumpUp);
                    }
                }
                break;

            case EventKeyboard::KeyCode::KEY_J: // 主武器攻击
                this->clearHitMonsters();
                _player->whenOnAttackKey(_player->_mainWeapon);
                break;

            case EventKeyboard::KeyCode::KEY_K: // 副武器攻击
                this->clearHitMonsters();
                _player->whenOnAttackKey(_player->_subWeapon);
                break;

            case EventKeyboard::KeyCode::KEY_S: // 下蹲或向下爬梯
                _player->_directionY = UpDownDirection::DOWN;
                _downPressed = true;
                if (_isAboveLadder || _player->_state == ActionState::hanging) {
                    _player->changeState(ActionState::hanging);
                    _player->changeState(ActionState::climbing);
                }
                else {
                    _player->changeState(ActionState::crouch);
                }
                break;

            case EventKeyboard::KeyCode::KEY_L: // 翻滚
                _player->changeState(ActionState::rollStart);
                break;

            case EventKeyboard::KeyCode::KEY_Q: // 切换武器
                _player->swapWeapon();
                break;

            case EventKeyboard::KeyCode::KEY_E: // 交互/购买武器
                if (_nearbyWeapon) {
                    int cost = _nearbyWeapon->getPrice();
                    if (cost == 0 || this->reduceGold(cost)) {
                        this->getNewWeapon();
                    }
                    else {
                        // 余额不足：弹出红色提示文字
                        if (_nearbyWeapon->getChildByName("NoMoneyTip"))
                            _nearbyWeapon->removeChildByName("NoMoneyTip");
                        auto tipLabel = Label::createWithTTF("Not Enough Gold!", "fonts/fusion-pixel.ttf", 18);
                        tipLabel->setColor(Color3B::RED);
                        tipLabel->setName("NoMoneyTip");
                        tipLabel->setPosition(Vec2(_nearbyWeapon->getContentSize().width / 2,
                            _nearbyWeapon->getContentSize().height + 60));
                        _nearbyWeapon->addChild(tipLabel);
                        auto spawn = Spawn::create(MoveBy::create(1.0f, Vec2(0, 30)), FadeOut::create(1.0f), nullptr);
                        tipLabel->runAction(Sequence::create(spawn, RemoveSelf::create(), nullptr));
                    }
                }
                break;
            default: break;
        }
        };

    // 释放按键逻辑
    listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_A) {
            _leftPressed = false;
            if (_player->_state == ActionState::run) _player->changeState(ActionState::idle);
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_D) {
            _rightPressed = false;
            if (_player->_state == ActionState::run) _player->changeState(ActionState::idle);
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_S) {
            _player->_directionY = UpDownDirection::NONE;
            _downPressed = false;
            if (_player->_state == ActionState::crouch) _player->changeState(ActionState::idle);
            if (_player->_state == ActionState::climbing) _player->changeState(ActionState::hanging);
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_W) {
            _player->_directionY = UpDownDirection::NONE;
            if (_player->_state == ActionState::climbing) _player->changeState(ActionState::hanging);
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// -----------------------------------------------------------------------------
// 游戏逻辑更新 (帧循环)
// -----------------------------------------------------------------------------

/**
 * 玩家受击入口。
 * @param sourcePos 伤害来源位置，用于计算受击朝向。
 */
void PlayerLayer::struck(float attackPower, Vec2 sourcePos) {
    float diffX = sourcePos.x - getPlayerWorldPosition().x;
    _player->changeDirection(diffX > 0 ? MoveDirection::RIGHT : MoveDirection::LEFT);
    _player->struck(attackPower);
}

/**
 * 处理捡起新武器并丢掉旧武器的逻辑。
 */
void PlayerLayer::getNewWeapon()
{
    if (!_nearbyWeapon || !_nearbyWeapon->getParent() || !_player) return;

    Vec2 dropPos = _player->getPosition();
    WeaponNode* weaponNodeToRemove = _nearbyWeapon;
    Weapon* newWpData = weaponNodeToRemove->pickUp();

    _nearbyWeapon = nullptr;
    Weapon* oldWpData = _player->getMainWeapon();
    _player->getNewWeapon(newWpData); // 玩家装备新武器

    // 丢弃旧武器，并给它一个抛物线动作
    if (oldWpData) {
        auto droppedNode = WeaponNode::create(oldWpData, dropPos);
        if (this->getParent()) {
            this->getParent()->addChild(droppedNode);
            float xOffset = (_player->_direction == MoveDirection::RIGHT) ? -40.0f : 40.0f;
            auto jump = JumpBy::create(0.4f, Vec2(xOffset, 20), 30, 1);
            droppedNode->runAction(jump);
        }
    }
}

/**
 * 每帧物理和逻辑轮询。
 */
void PlayerLayer::update(float dt) {
    auto body = _player->getPhysicsBody();
    if (_player->isLethalState()) {
        _player->update(dt);
        return;
    }

    // 1. 梯子脱离逻辑
    if (_player->_state == ActionState::climbing || _player->_state == ActionState::hanging) {
        // 爬到地面底部或离开梯子范围时恢复重力
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

    // 2. 边缘攀爬逻辑处理
    if (_player->_state == ActionState::climbedge && !_isPassingPlatform) {
        body->setGravityEnable(true);
        body->setVelocity(Vec2::ZERO);
        _player->changeState(ActionState::idle);
    }

    // 3. 持续移动处理 (处理 A/D 键同时按下或单按的连贯性)
    if (_leftPressed && !_rightPressed) {
        _player->changeDirection(MoveDirection::LEFT);
        if (_player->_state == ActionState::jumpDown || _player->_state == ActionState::jumpUp) {
            _player->giveVelocityX(_player->_runSpeed); // 空中左右微调
        }
        _player->changeState(ActionState::run);
    }
    else if (_rightPressed && !_leftPressed) {
        _player->changeDirection(MoveDirection::RIGHT);
        if (_player->_state == ActionState::jumpDown || _player->_state == ActionState::jumpUp) {
            _player->giveVelocityX(_player->_runSpeed);
        }
        _player->changeState(ActionState::run);
    }
    else {
        // 无左右输入时，空中水平惯性清零（可选）
        if (_player->_state == ActionState::jumpDown || _player->_state == ActionState::jumpUp) {
            _player->set0VelocityX();
        }
    }

    // 4. 重置跳跃计数
    if (_player->_state == ActionState::idle || _player->_state == ActionState::run) {
        _jumpCount = 0;
    }

    _player->update(dt); // 最后调用玩家自身的逻辑更新
}