#include "Player.h"
#include "Weapon.h"
#include "Shield.h"
#include "Bow.h"
#include "Sword.h"
USING_NS_CC;
const float GRAVITY = 980.0f;
//*******************************************************************
//*******************************************************************
//*******************************************************************
//初始化
bool Player::init()
{
    if (!Sprite::initWithFile("Graph/Player/playerInit.png", Rect(0, 0, 150, 150)))
        return false;

	_moveSpeed = 200.0f;
	_jumpSpeed = 250.0f;
	_rollSpeed = 300.0f;
	_state = ActionState::stand;
	_direction = MoveDirection::RIGHT;
    this->setOriginalAttributes(BasicAttributes({ 100, 10, 10 }));
    this->setFinalAttributes(BasicAttributes({ 100, 10, 10 }));
    this->setLevel(1);


    this->_mainWeapon = new Sword(Sword::SwordType::BaseballBat);
    this->_subWeapon = NULL;

    
    Size bodySize = Size(50, 75);
    float bottomPadding = 75.0f;
    float offsetY = -(this->getContentSize().height / 2 - bodySize.height / 2 - bottomPadding);

    auto body = PhysicsBody::createBox(bodySize,
        PhysicsMaterial(0.1f, 0.0f, 0.5f),
        Vec2(0, offsetY));

    body->setDynamic(true);
    body->setRotationEnable(false);
    body->setGravityEnable(true);
    body->setVelocity(Vec2::ZERO);
    this->setPhysicsBody(body);


    body->setCategoryBitmask(PLAYER_BODY);
    body->setCollisionBitmask(GROUND | ENEMY_BODY);
    body->setContactTestBitmask(ENEMY_BODY);

    this->createHurtBox();

    this->scheduleUpdate();
    return true;
}

//*******************************************************************
//*******************************************************************
//*******************************************************************
//动作
void Player::getWeapon(Weapon* w)
{
    if (!_mainWeapon)
        _mainWeapon = w;
    else if (!_subWeapon)
        _subWeapon = w;
    this->setFinalAttributes({ this->getOriginalAttributes().health + w->getWeaponAttributes().health,
                               this->getOriginalAttributes().attack + w->getWeaponAttributes().attack,
                               this->getOriginalAttributes().defense + w->getWeaponAttributes().defense});
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
    if (!body)
        return;
    int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
    Vec2 velocity = body->getVelocity();
    velocity.x = _moveSpeed * dir;
    body->setVelocity(velocity);
	this->setFlippedX(dir == -1);
}
void Player::jumpDown()
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
    this->createAttackBox();
    if (_state == ActionState::jumpUp || _state == ActionState::jumpDown)
    {
        return;
    }
    else
    {
        this->stand();
    }
}
void Player::atkB()
{
    this->createAttackBox();
    this->stand();
}
void Player::atkBackStabber()
{
    this->createAttackBox();
    this->stand();
}
void Player::AtkBaseballBatA()
{
    this->createAttackBox();
    this->stand();
}
void Player::AtkBaseballBatB()
{
    this->createAttackBox();
    this->stand();
}
void Player::AtkBaseballBatC()
{
    this->createAttackBox();
    this->stand();
}
void Player::AtkBaseballBatD()
{
    this->createAttackBox();
    this->stand();
}
void Player::AtkBaseballBatE()
{
    this->createAttackBox();
    this->stand();
}
void Player::atkBroadSwordA()
{
    this->createAttackBox();
    this->stand();
}
void Player::AtkcloseCombatBow()
{
    this->createAttackBox();
    this->stand();
}
void Player::AtkdualBow()
{
    this->createAttackBox();
	this->stand();
}
void Player::crossbowShoot()
{
    this->stand();
}
void Player::dead()
{
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
//动画
void Player::changeState(ActionState newState)
{
    if (!whetherCanChangeToNewState(newState))
        return;
    if (_state == newState)
        return;
    switch (newState)
    {
        case ActionState::jumpDown:; break;
        case ActionState::stand:this->stand(); break;
        case ActionState::run:this->run(); break;
        case ActionState::jumpUp:this->jumpUp(); break;
        case ActionState::rollStart:this->rollStart(); break;
        case ActionState::crouch:this->crouch(); break;
		case ActionState::atkA:this->atkA(); break;
		case ActionState::atkB:this->atkB(); break;
		case ActionState::atkBackStabber:this->atkBackStabber(); break;
		case ActionState::AtkBaseballBatA:this->AtkBaseballBatA(); break;
		case ActionState::AtkBaseballBatB:this->AtkBaseballBatB(); break;
		case ActionState::AtkBaseballBatC:this->AtkBaseballBatC(); break;
		case ActionState::AtkBaseballBatD:this->AtkBaseballBatD(); break;
		case ActionState::AtkBaseballBatE:this->AtkBaseballBatE(); break;
		case ActionState::atkBroadSwordA:this->atkBroadSwordA(); break;
		case ActionState::AtkcloseCombatBow:this->AtkcloseCombatBow(); break;
		case ActionState::AtkdualBow:this->AtkdualBow(); break;
		case ActionState::crossbowShoot:this->crossbowShoot(); break;
        default:break;
    }
    if(isAttackState(_state)&&newState==ActionState::run)
    {
        int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
        this->setFlippedX(dir == -1);
        return;
	}
    _state = newState;
    bool loop = StateTable[newState].loop;
    playAnimation(newState, loop);
}
void Player::changeStateByWeapon(Weapon* weapon)
{
    if (isAttackState(_state))
        return; 
    if (!weapon)
    {
        changeState(ActionState::atkA);
        return;
    }
    Sword* sword = dynamic_cast<Sword*>(weapon);
    if (sword)
    {
		if (sword->getSwordType() == Sword::SwordType::BackStabber)
            changeState(ActionState::atkBackStabber);
        else if (sword->getSwordType() == Sword::SwordType::BaseballBat)
            changeState(ActionState::AtkBaseballBatA);
        else if (sword->getSwordType() == Sword::SwordType::BroadSword)
			changeState(ActionState::atkBroadSwordA);
        return;
    }
    Bow* bow = dynamic_cast<Bow*>(weapon);
    if (bow)
    {
        if(bow->getBowType()==Bow::BowType::closeCombatBow)
            changeState(ActionState::AtkcloseCombatBow);
        else if(bow->getBowType() == Bow::BowType::dualBow)
			changeState(ActionState::AtkdualBow);
        else if(bow->getBowType() == Bow::BowType::crossbow)
			changeState(ActionState::crossbowShoot);
		return;
    }

}
bool Player::whetherCanChangeToNewState(ActionState newState) const
{
    if (isAttackState(_state) && isAttackState(newState))
        return true;
    if (newState == ActionState::stand)
        return true;
    if (_state == ActionState::stand || _state == ActionState::run)
        return true;
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
    this->stopActionByTag(1001);
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
                actionWhenEnding(state);
            }
            });
        action = Sequence::create(animate, callback, nullptr);
    }

    action->setTag(1001);
    this->runAction(action);
}
cocos2d::Animation* Player::createAnim(const std::string& name, int frameCount, float time) const
{
    auto anim = Animation::create();

    // 设置目标裁剪尺寸
    const float targetWidth  = 150.0f;
    const float targetHeight = 150.0f;

    for (int i = 0; i < frameCount; ++i)
    {
        std::string framePath = StringUtils::format("Graph/Player/%s_%02d-=-0-=-.png", name.c_str(), i);

        // 先创建一个临时Sprite获取图片原始尺寸
        auto tempSprite = Sprite::create(framePath);
        if (!tempSprite) continue; // 图片不存在则跳过

        auto originalSize = tempSprite->getContentSize();

        // 计算截取偏移，使目标区域居中
        float offsetX = (originalSize.width  - targetWidth)  / 2.0f;
        float offsetY = (originalSize.height - targetHeight) / 2.0f;

        // 构建SpriteFrame并添加到动画
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, offsetY, targetWidth, originalSize.height));
        anim->addSpriteFrame(frame);
    }

    anim->setDelayPerUnit(time / frameCount);
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
        case ActionState::stand:            anim = createAnim("stand", 1, 1.0f); break;
        case ActionState::run:              anim = createAnim("run", 20, 1.0f); break;
        case ActionState::jumpDown:         anim = createAnim("jumpDown", 4, 1.0f); break;
        case ActionState::jumpUp:           anim = createAnim("jumpUp", 6, 1.0f); break;
        case ActionState::crouch:           anim = createAnim("crouch", 1, 0.2f); break;
        case ActionState::rollStart:        anim = createAnim("rollStart", 4,0.5f); break;
        case ActionState::atkA:             anim = createAnim("atkA", 14, 0.25f); break;
		case ActionState::atkB:             anim = createAnim("atkB", 10, 0.25f); break;
		case ActionState::atkBackStabber:   anim = createAnim("atkBackStabberA", 13, 0.25f); break;
		case ActionState::AtkBaseballBatA:  anim = createAnim("AtkBaseballBatA", 11, 0.25f); break;
        case ActionState::AtkBaseballBatB:  anim = createAnim("AtkBaseballBatB", 12, 0.25f); break;
        case ActionState::AtkBaseballBatC:  anim = createAnim("AtkBaseballBatC", 10, 0.25f); break;
        case ActionState::AtkBaseballBatD:  anim = createAnim("AtkBaseballBatD", 14, 0.25f); break;
        case ActionState::AtkBaseballBatE:  anim = createAnim("AtkBaseballBatE", 8, 0.25f); break;
		case ActionState::atkBroadSwordA:   anim = createAnim("atkBroadSwordA", 15, 0.25f); break;
        case ActionState::AtkcloseCombatBow:anim = createAnim("closeCombatBow", 25, 0.25f); break;
        case ActionState::AtkdualBow:       anim = createAnim("dualBow", 25, 0.25f); break;
        case ActionState::crossbowShoot:    anim = createAnim("crossbowShoot", 11, 0.25f); break;
        default:return nullptr;
    }
    anim->retain();
    _animationCache[state] = anim;
    return anim;
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
//攻击受击判定
void Player::whenOnAttackKey(Weapon* w)
{
    if (_state == ActionState::stand ||
        _state == ActionState::jumpUp)
    {
        this->changeStateByWeapon(w);
        return;
    }
    if (_state == ActionState::run)
    {
        this->changeState(ActionState::stand);
        this->changeStateByWeapon(w);
		return;
    }

    // 连击输入判定
    if (_state == ActionState::atkA || _state == ActionState::AtkBaseballBatA)
    {
        _comboStep = 1;
        _comboInput = true;
    }
    else if (_state == ActionState::AtkBaseballBatB || _state == ActionState::AtkBaseballBatC || _state == ActionState::AtkBaseballBatD)
    {
        _comboInput = true;
    }
}

void Player::actionWhenEnding(ActionState state)
{
    // Combo处理
    if (_state == state && _comboInput)
    {
        _comboInput = false;
        if (state == ActionState::atkA && _comboStep == 1)
        {
            _comboStep++;
            changeState(ActionState::atkB);
            return;
        }
        else if (state == ActionState::AtkBaseballBatA && _comboStep == 1)
        {
            _comboStep++;
            changeState(ActionState::AtkBaseballBatB);
            return;
        }
        else if (state == ActionState::AtkBaseballBatB && _comboStep == 2)
        {
            _comboStep++;
            changeState(ActionState::AtkBaseballBatC);
            return;
        }
        else if (state == ActionState::AtkBaseballBatC && _comboStep == 3)
        {
            _comboStep++;
            changeState(ActionState::AtkBaseballBatD);
            return;
        }
    }

    // 下落状态判断
    if (_velocity.y < 0)
    {
        changeState(ActionState::jumpDown);;
        return;
    }

    // 重置状态
    _comboStep = 0;
    _comboInput = false;
    changeState(ActionState::stand);
}
bool Player::isAttackState(ActionState s) const
{
    return s == ActionState::atkA ||
           s == ActionState::atkB ||
           s == ActionState::AtkBaseballBatA ||
           s == ActionState::AtkBaseballBatB ||
           s == ActionState::AtkBaseballBatC ||
           s == ActionState::AtkBaseballBatD ||
           s == ActionState::AtkBaseballBatE;
}
void Player::struck(int value)
{
    BasicAttributes currentAttributes = this->getFinalAttributes();
    currentAttributes.health -= value;
    if (currentAttributes.health < 0)
    {
        currentAttributes.health = 0;
        this->dead();
    }
    this->setFinalAttributes(currentAttributes);
}
void Player::createHurtBox()
{
    // Player::createHurtBox()
    _hurtNode = Node::create();
    auto hurtBody = PhysicsBody::createBox(Size(50, 75));
    _hurtNode->setPosition(Vec2(75, 112));
    hurtBody->setDynamic(false);
    hurtBody->setGravityEnable(false);
    hurtBody->setRotationEnable(false);
    hurtBody->setCategoryBitmask(PLAYER_HURT);    // PLAYER_HURT
    hurtBody->setCollisionBitmask(0);      // 不产生物理碰撞
    hurtBody->setContactTestBitmask(ENEMY_ATTACK); // ENEMY_HIT

    _hurtNode->setPhysicsBody(hurtBody);
    this->addChild(_hurtNode);
}
void Player::createAttackBox()
{
    removeAttackBox(); 

    _attackNode = Node::create();
    _attackNode->setPosition(Vec2(75, 75));
    this->addChild(_attackNode, 10);

    //获取方向偏移
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;


    float boxWidth = 80.0f;
    float boxHeight = 50.0f;
    float offsetX = 20.0f * dir; // 向右为 +40，向左为 -40

    // 计算矩形的左下角和右上角
    Vec2 origin = Vec2(offsetX - boxWidth / 2, -boxHeight / 2);
    Vec2 destination = Vec2(offsetX + boxWidth / 2, boxHeight / 2);


    auto attackBody = PhysicsBody::createBox(Size(boxWidth, boxHeight),
        PhysicsMaterial(0, 0, 0),
        Vec2(offsetX, 35)); 
    attackBody->setDynamic(false);
    attackBody->setGravityEnable(false);
    attackBody->setCategoryBitmask(PLAYER_ATTACK);
    attackBody->setCollisionBitmask(0);
    attackBody->setContactTestBitmask(ENEMY_HURT);
    _attackNode->setPhysicsBody(attackBody);

    //延长显示时间以便调试 (例如 0.5s)
    _attackNode->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() { this->removeAttackBox(); }),
        nullptr
    ));
}
void Player::removeAttackBox()
{
    if (_attackNode)
    {
        _attackNode->removeFromParent();
        _attackNode = nullptr;
    }
}