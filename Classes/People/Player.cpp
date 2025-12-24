#include "Player.h"
#include "Weapon.h"
#include "Shield.h"
#include "Bow.h"
#include "Sword.h"
const float targetWidth = 100.0f;
const float targetHeight = 200.0f;
USING_NS_CC;
const float GRAVITY = 980.0f;
//*******************************************************************
//*******************************************************************
//*******************************************************************
//初始化
bool Player::init()
{
    if (!Sprite::initWithFile("Graph/Player/idle_00-=-0-=-.png", Rect(0, 0, targetWidth, targetHeight)))
        return false;
	_runSpeed = 200.0f;
	_jumpSpeed = 500.0f;
	_rollSpeed = 600.0f;
	_state = ActionState::idle;
	_direction = MoveDirection::RIGHT;
    this->setOriginalAttributes(BasicAttributes({ 100, 10, 10 }));
    this->setFinalAttributes(BasicAttributes({ 100, 10, 10 }));
    this->setLevel(1);


    this->_mainWeapon = new Sword(Sword::SwordType::OvenAxe);
    this->_subWeapon = new Shield(Shield::ShieldType::LightningShield);

    
  
    auto size = this->getContentSize();
    //中心点在图片中间
    auto body = PhysicsBody::createBox(cocos2d::Size(targetWidth/3,targetHeight/3), PhysicsMaterial(0.1f, 0.0f, 0.5f),Vec2(0,1.0/6*targetHeight));
	//auto body = PhysicsBody::createBox(cocos2d::Size(targetWidth,targetHeight), PhysicsMaterial(0.1f, 0.0f, 0.5f));
    body->setDynamic(true);
    body->setRotationEnable(false);
    body->setGravityEnable(true);
    body->setVelocity(Vec2::ZERO);
    this->setPhysicsBody(body);


    body->setCategoryBitmask(PLAYER_BODY);
    body->setCollisionBitmask(GROUND | ENEMY_BODY | PLATFORM | LADDER);
    body->setContactTestBitmask(ENEMY_BODY | PLATFORM | LADDER);

    this->createHurtBox();
    playAnimation(ActionState::idle, true);
    this->scheduleUpdate();
    return true;
}
void Player::giveVelocityX(float speed)
{
	int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
	this->getPhysicsBody()->setVelocity(Vec2(speed * dir, this->getPhysicsBody()->getVelocity().y));
}
void Player::changeDirection(MoveDirection dir)
{
    _direction = dir;
    this->setFlippedX(_direction == MoveDirection::LEFT);
}
void Player::set0VelocityX()
{
    auto body = this->getPhysicsBody();
    if (!body)
        return;
    Vec2 currentVel = body->getVelocity();
    currentVel.x = 0;
	body->setVelocity(Vec2(0, currentVel.y));
}
void Player::set0VelocityY()
{
    auto body = this->getPhysicsBody();
    if (!body)
        return;
    Vec2 currentVel = body->getVelocity();
    currentVel.y = 0;
    body->setVelocity(Vec2(currentVel.x, 0));
}
bool Player::isOnGround() const
{
    return abs(this->getPhysicsBody()->getVelocity().y) < 1.0f;
}
void Player::update(float dt)
{
    // 应用重力
    auto body = this->getPhysicsBody();
    if (this->isOnGround() && (_state == ActionState::jumpDown || _state == ActionState::jumpUp))
        this->changeState(ActionState::idle);
    else if (!this->isOnGround())
        this->changeState(ActionState::jumpDown);
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
void Player::idle()
{
    this->set0VelocityX();
}
void Player::run()
{

    auto body = this->getPhysicsBody();
    if (!body)
        return;
	this->giveVelocityX(_runSpeed);
}
void Player::jumpDown()
{

}
void Player::jumpUp()
{
    auto body = this->getPhysicsBody();
    if (!body)
        return;
    Vec2 impulse(0, body->getMass() * _jumpSpeed);
    body->applyImpulse(impulse);

}
void Player::rollStart()
{
    this->createRollBox();
    auto body = this->getPhysicsBody();
    if (!body)
        return;
    this->giveVelocityX(_rollSpeed);
}
void Player::crouch()
{
}
void Player::AtkcloseCombatBow()
{
    this->idle();
}
void Player::AtkdualBow()
{
}
void Player::crossbowShoot()
{
}
void Player::dead()
{
    this->changeState(ActionState::lethalFall);
}
void Player::shootArrow()
{
	int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
    auto arrow = dynamic_cast<Arrow*>(FlyingObject::create(FlyType::Arrow, true));
	arrow->setPosition(this->getPosition()+Vec2(0,50));
    if (!arrow)
		return;
    this->getParent()->addChild(arrow);
    arrow->run(Vec2(dir, 0));
}
void Player::throwBomb()
{
    int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
    auto bomb = dynamic_cast<Bomb*>(FlyingObject::create(FlyType::Bomb, true));
    bomb->setPosition(this->getPosition() + Vec2(0, 50));
    if (!bomb)
        return;
    this->getParent()->addChild(bomb);
    bomb->run(Vec2(20, 5));
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
//动画
void Player::changeState(ActionState newState)
{
    if (_state == newState) return; // 如果已经是该状态，直接返回，防止动画重置
    if (!whetherCanChangeToNewState(newState)) return;
    switch (newState)
    {
        case ActionState::jumpDown:; break;
        case ActionState::idle:this->idle(); break;
        case ActionState::run:this->run(); break;
        case ActionState::jumpUp:this->jumpUp(); break;
        case ActionState::rollStart:this->rollStart(); break;
        case ActionState::crouch:this->crouch(); break;


		case ActionState::atkA:this->createAttackBox(); break;
		case ActionState::atkB:this->createAttackBox(); break;
		case ActionState::atkBackStabber:this->createAttackBox(); break;
		case ActionState::AtkBaseballBatA:this->createAttackBox(); break;
		case ActionState::AtkBaseballBatB:this->createAttackBox(); break;
		case ActionState::AtkBaseballBatC:this->createAttackBox(); break;
		case ActionState::AtkBaseballBatD:this->createAttackBox(); break;
		case ActionState::AtkBaseballBatE:this->createAttackBox(); break;
		case ActionState::atkBroadSwordA:this->createAttackBox(); break;
        case ActionState::atkBroadSwordB:this->createAttackBox(); break;
        case ActionState::atkBroadSwordC:this->createAttackBox(); break;
        case ActionState::AtkOvenAxeA:this->createAttackBox(); break;
        case ActionState::AtkOvenAxeB:this->createAttackBox(); break;
        case ActionState::AtkOvenAxeC:this->createAttackBox(); break;

		case ActionState::AtkcloseCombatBow:this->AtkcloseCombatBow(); break;
		case ActionState::AtkdualBow:this->AtkdualBow(); break;
		case ActionState::crossbowShoot:this->crossbowShoot(); break;

        case ActionState::blockEndLightningShield:this->createBlockEndBox(); break;
        case ActionState::blockEndParryShield:this->createBlockEndBox(); break;
        default:break;
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
        else if (sword->getSwordType() == Sword::SwordType::OvenAxe)
            changeState(ActionState::AtkOvenAxeA);
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
    Shield* shield = dynamic_cast<Shield*>(weapon);
    if (shield)
    {
        if (shield->getShieldType() == Shield::ShieldType::LightningShield)
            changeState(ActionState::blockEndLightningShield);
        else if (shield->getShieldType() == Shield::ShieldType::ParryShield)
            changeState(ActionState::blockEndParryShield);
        return;
    }

}
bool Player::whetherCanChangeToNewState(ActionState newState) const
{

    if (newState == ActionState::idle) 
        return true;
    if (isAttackState(_state) && isAttackState(newState)) 
        return true;
    auto currentConfig = StateTable[_state];
    auto newConfig = StateTable[newState];

    if (!currentConfig.canBeInterrupted) return false;
    if (newConfig.priority < currentConfig.priority) return false;

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
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, offsetY, targetWidth, targetHeight));
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
        case ActionState::idle:            anim = createAnim("idle", 46, 0.5f); break;
        case ActionState::run:              anim = createAnim("run", 20, 0.5f); break;
        case ActionState::jumpDown:         anim = createAnim("jumpDown", 4, 0.3f); break;
        case ActionState::jumpUp:           anim = createAnim("jumpUp", 6, 0.3f); break;
        case ActionState::crouch:           anim = createAnim("crouch", 1, 0.3f); break;
        case ActionState::rollStart:        anim = createAnim("rollStart", 4,0.3f); break;
        case ActionState::atkA:             anim = createAnim("atkA", 14, 0.3f); break;
		case ActionState::atkB:             anim = createAnim("atkB", 10, 0.3f); break;
		case ActionState::atkBackStabber:   anim = createAnim("atkBackStabberA", 13, 0.3f); break;
		case ActionState::AtkBaseballBatA:  anim = createAnim("AtkBaseballBatA", 11, 0.3f); break;
        case ActionState::AtkBaseballBatB:  anim = createAnim("AtkBaseballBatB", 12, 0.3f); break;
        case ActionState::AtkBaseballBatC:  anim = createAnim("AtkBaseballBatC", 10, 0.3f); break;
        case ActionState::AtkBaseballBatD:  anim = createAnim("AtkBaseballBatD", 14, 0.3f); break;
        case ActionState::AtkBaseballBatE:  anim = createAnim("AtkBaseballBatE", 8, 0.3f); break;
		case ActionState::atkBroadSwordA:   anim = createAnim("atkBroadSwordA", 15, 0.5f); break;
        case ActionState::atkBroadSwordB:   anim = createAnim("atkBroadSwordB", 16, 0.5f); break;
        case ActionState::atkBroadSwordC:   anim = createAnim("atkBroadSwordC", 28, 1.0f); break;
        case ActionState::AtkOvenAxeA:      anim = createAnim("AtkOvenAxeA", 38, 1.0f); break;
        case ActionState::AtkOvenAxeB:      anim = createAnim("AtkOvenAxeB", 40, 1.0f); break;
        case ActionState::AtkOvenAxeC:      anim = createAnim("AtkOvenAxeC", 48, 1.0f); break;
        case ActionState::AtkcloseCombatBow:anim = createAnim("closeCombatBow", 25, 0.3f); break;
        case ActionState::AtkdualBow:       anim = createAnim("dualBow", 25, 0.3f); break;
        case ActionState::crossbowShoot:    anim = createAnim("crossbowShoot", 11, 0.3f); break;
        case ActionState::lethalSlam:       anim = createAnim("lethalSlam", 17, 0.5f); break;
        case ActionState::lethalFall:       anim = createAnim("lethalFall", 11, 0.3f); break;
        case ActionState::blockEndLightningShield: anim = createAnim("blockEndLightningShield", 9, 0.2f); break;
        case ActionState::blockEndParryShield: anim = createAnim("blockEndParryShield", 8, 0.2f); break;
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
    if (isAttackState(_state))
    {
        _comboInput = true;
        return;
    }
    _comboStep = 1;
    _comboInput = false; 
    this->set0VelocityX();
    this->changeStateByWeapon(w);
}

void Player::actionWhenEnding(ActionState state)
{
    // 远程武器处理
    if (state == ActionState::crossbowShoot || state == ActionState::AtkcloseCombatBow || state == ActionState::AtkdualBow)
    {
        this->throwBomb();
    }

    // --- 连招衔接判定 ---
    if (_comboInput)
    {
        ActionState nextState = ActionState::idle;

        if (state == ActionState::atkA) nextState = ActionState::atkB;
        else if (state == ActionState::AtkBaseballBatA) nextState = ActionState::AtkBaseballBatB;
        else if (state == ActionState::AtkBaseballBatB) nextState = ActionState::AtkBaseballBatC;
        else if (state == ActionState::AtkBaseballBatC) nextState = ActionState::AtkBaseballBatD;
        else if (state == ActionState::AtkBaseballBatD) nextState = ActionState::AtkBaseballBatE;
        else if (state == ActionState::atkBroadSwordA)  nextState = ActionState::atkBroadSwordB;
        else if (state == ActionState::atkBroadSwordB)  nextState = ActionState::atkBroadSwordC;
        else if (state == ActionState::AtkOvenAxeA)     nextState = ActionState::AtkOvenAxeB;
        else if (state == ActionState::AtkOvenAxeB)     nextState = ActionState::AtkOvenAxeC;

        if (nextState != ActionState::idle)
        {
            _comboStep++;
            _comboInput = false;
            this->changeState(nextState); 
            return; 
        }
    }
    if (state == ActionState::rollStart)
    {
        this->createHurtBox(); // 恢复正常的受击盒子
    }
    if (state == ActionState::lethalFall)
    {
        this->changeState(ActionState::lethalSlam);
    }
    _comboStep = 0;
    _comboInput = false;

    if (!this->isOnGround()) 
    {
        this->changeState(ActionState::jumpDown);
    }
    else 
    {
        this->changeState(ActionState::idle);
    }
}
bool Player::isAttackState(ActionState s) const
{
    return s == ActionState::atkA ||
           s == ActionState::atkB ||
           s == ActionState::AtkBaseballBatA ||
           s == ActionState::AtkBaseballBatB ||
           s == ActionState::AtkBaseballBatC ||
           s == ActionState::AtkBaseballBatD ||
           s == ActionState::atkBroadSwordA ||
           s == ActionState::atkBroadSwordB ||
           s == ActionState::atkBroadSwordC ||
           s == ActionState::AtkOvenAxeA ||
           s == ActionState::AtkOvenAxeB ||
           s == ActionState::AtkOvenAxeC ||
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
    removeHurtBox();
    // Player::createHurtBox()
    _hurtNode = Node::create();
    //中心点是图片的左下角(0,0)，因为hurtBox是player的子节点
    auto hurtBody = PhysicsBody::createBox(cocos2d::Size(targetWidth / 3, targetHeight / 3), PhysicsMaterial(0, 0, 0), Vec2(targetWidth/2, targetHeight*2/3));
    hurtBody->setDynamic(false);
    hurtBody->setGravityEnable(false);
    hurtBody->setRotationEnable(false);
    hurtBody->setCategoryBitmask(PLAYER_HURT);    // PLAYER_HURT
    hurtBody->setCollisionBitmask(0);      // 不产生物理碰撞
    hurtBody->setContactTestBitmask(ENEMY_ATTACK); // ENEMY_HIT

    _hurtNode->setPhysicsBody(hurtBody);
    this->addChild(_hurtNode);
}
void Player::createRollBox()
{
    removeHurtBox();
    _hurtNode = Node::create();
    DelayTime::create(0.2f);
    auto hurtBody = PhysicsBody::createBox(cocos2d::Size(targetWidth / 3, targetHeight / 6), PhysicsMaterial(0, 0, 0), Vec2(targetWidth / 2, targetHeight * 7 / 12));
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
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;
    _attackNode->setPosition(Vec2(targetWidth/2+dir*targetWidth/6, targetHeight*2/3));
    this->addChild(_attackNode, 10);

    //获取方向偏移
   


    auto attackBody = PhysicsBody::createBox(cocos2d::Size(targetWidth / 3, targetHeight / 6), PhysicsMaterial(0, 0, 0));
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
void Player::createBlockEndBox()
{
    removeAttackBox();
    _attackNode = Node::create();
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;
    _attackNode->setPosition(Vec2(targetWidth / 2 + dir * targetWidth / 6, targetHeight * 2 / 3));
    this->addChild(_attackNode, 10);

    //获取方向偏移



    auto attackBody = PhysicsBody::createBox(cocos2d::Size(targetWidth / 3, targetHeight / 6), PhysicsMaterial(0, 0, 0));
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
void Player::removeHurtBox()
{
    if (_hurtNode)
    {
        _hurtNode->removeFromParent();
        _hurtNode = nullptr;
    }
}
void Player::removeAttackBox()
{
    if (_attackNode)
    {
        _attackNode->removeFromParent();
        _attackNode = nullptr;
    }
}