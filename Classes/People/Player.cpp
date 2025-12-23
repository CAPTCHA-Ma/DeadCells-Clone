#include "Player.h"
#include "Weapon.h"
#include "Shield.h"
#include "Bow.h"
#include "Sword.h"
const float targetWidth = 200.0f;
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
    this->setFinalAttributes(BasicAttributes({ 100, 100, 100 }));
    this->setLevel(1);


    this->_mainWeapon = new Sword(Sword::SwordType::BaseballBat);
    this->_subWeapon = new Bow(Bow::BowType::dualBow);
    this->createNormalBody();
    
  
    

    this->createHurtBox();
    playAnimation(ActionState::idle, true);
    this->scheduleUpdate();
    return true;
}
void Player::setupBodyProperties(cocos2d::PhysicsBody* body) 
{
    body->setDynamic(true);
    body->setRotationEnable(false);
    body->setGravityEnable(true);
    body->setCategoryBitmask(PLAYER_BODY);
    body->setCollisionBitmask(GROUND);
}
void Player::updatePhysicsBody(const cocos2d::Size& size, const cocos2d::Vec2& offset) 
{
    auto currentBody = this->getPhysicsBody();
    if (currentBody) 
    {
        currentBody->removeAllShapes();
        auto newShape = PhysicsShapeBox::create(size, PhysicsMaterial(0.1f, 0.0f, 0.5f), offset);
        currentBody->addShape(newShape);
    }
    else {
        auto body = PhysicsBody::createBox(size, PhysicsMaterial(0.1f, 0.0f, 0.5f), offset);
        this->setPhysicsBody(body);
    }

    setupBodyProperties(this->getPhysicsBody());
}
void Player::createNormalBody() 
{
    updatePhysicsBody(Size(targetWidth / 3, targetHeight / 3), Vec2(0, targetHeight / 6));
}
void Player::createRollBody() 
{
    // 翻滚状态：高度减半，重心降低
    updatePhysicsBody(Size(targetWidth / 3, targetHeight / 6), Vec2(0, targetHeight / 12));
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
    else if (!this->isOnGround()&&_state!= ActionState::rollStart)
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
    // --- 核心修复：只有在地面上才能起跳 ---
    if (!this->isOnGround()) {
        return;
    }

    auto body = this->getPhysicsBody();
    if (!body) return;

    // 给予向上的冲量
    Vec2 impulse(0, body->getMass() * _jumpSpeed);
    body->applyImpulse(impulse);

    // 立即手动切换状态，防止重复起跳
    // _state = ActionState::jumpUp; 
}
void Player::rollStart()
{
    this->createRollBody();
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
    this->unscheduleUpdate();
    _invincible = true;

    // 2. 移除攻击和受击判定
    this->removeAttackBox();
    this->removeHurtBox();

    // 3. 停止物理移动（可选，如果你希望死后滑行一段则不设为ZERO）
    this->getPhysicsBody()->setVelocity(Vec2(this->getPhysicsBody()->getVelocity().x * 0.5f, 0));

    // 4. 开始死亡序列
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
    if (_state == ActionState::lethalSlam)
        return;
    if (_state == ActionState::lethalFall && newState != ActionState::lethalSlam) 
        return;
    if (_state == newState)
        return; 
    if (!whetherCanChangeToNewState(newState))
        return;
    switch (newState)
    {
        case ActionState::jumpDown:; break;
        case ActionState::idle:this->idle(); break;
        case ActionState::run:this->run(); break;
        case ActionState::jumpUp:this->jumpUp(); break;
        case ActionState::rollStart:this->rollStart(); break;
        case ActionState::crouch:this->crouch(); break;
        case ActionState::dead:this->dead(); break;

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

        case ActionState::blockEndLightningShield:this->createShieldParryBox(); break;
        case ActionState::blockEndParryShield:this->createShieldParryBox(); break;
        default:break;
    }
    _state = newState;
    if (_state == ActionState::dead)
        return;
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
    if(!isOnGround()&&newState==ActionState::rollStart)
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
        case ActionState::idle:             anim = createAnim("idle", 46, 0.5f); break;
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
        case ActionState::lethalFall:       anim = createAnim("lethalFall", 11, 1.0f); break;
        case ActionState::lethalSlam:       anim = createAnim("lethalSlam", 17, 1.0f); break;
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
    if (state == ActionState::lethalFall)
    {
        if (this->getPhysicsBody()) {
            this->getPhysicsBody()->setVelocity(Vec2::ZERO);
            this->getPhysicsBody()->setGravityEnable(false); 
        }
        this->changeState(ActionState::lethalSlam);
        return; 
    }
    if (state == ActionState::lethalSlam)
    {
        CCLOG("DEAD ANIMATION FINISHED - SHOW UI");
        // 可以在这里发送一个自定义事件，通知 GameScene 弹出结算界面
        return;
    }

    if (state == ActionState::crossbowShoot || state == ActionState::AtkcloseCombatBow || state == ActionState::AtkdualBow)
    {
        this->shootArrow();
        //this->throwBomb();
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
    _comboStep = 0;
    _comboInput = false;
    if (_state != ActionState::lethalFall && _state != ActionState::lethalSlam)
    {
        if (state == ActionState::rollStart)
        {
            _invincible = false; 
            this->setOpacity(255);
            this->createNormalBody(); 
        }

        if (!this->isOnGround()) {
            this->changeState(ActionState::jumpDown);
        }
        else {
            this->changeState(ActionState::idle);
        }
        if (!this->isOnGround())
        {
            this->changeState(ActionState::jumpDown);
        }
        else
        {
            this->changeState(ActionState::idle);
        }
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
void Player::struck(float attackPower) {
    auto attributes = this->getFinalAttributes();
    attributes.health -= attackPower * (100 - attributes.defense)/100;

    this->setFinalAttributes(attributes);

    CCLOG("Player HP: %f", this->getFinalAttributes().health);

    if (this->getFinalAttributes().health <= 0) 
        this->dead();
}
void Player::createHurtBox()
{
    if (_hurtNode) 
        return; // 如果已经存在，不要重复创建
    _hurtNode = Node::create();
    auto hurtBody = PhysicsBody::createBox(cocos2d::Size(targetWidth / 3, targetHeight / 3), PhysicsMaterial(0, 0, 0), Vec2(targetWidth/2, targetHeight*2/3));

    hurtBody->setDynamic(false);
    hurtBody->setCategoryBitmask(PLAYER_HURT);
    hurtBody->setCollisionBitmask(0);
    hurtBody->setContactTestBitmask(ENEMY_ATTACK);

    _hurtNode->setPhysicsBody(hurtBody);
    this->addChild(_hurtNode);
}
void Player::startRollInvincible(float time)
{
    this->stopActionByTag(2001);

    _invincible = true;

    auto delay = DelayTime::create(time);
    auto endInvincible = CallFunc::create([this]() {
        _invincible = false;
        this->setOpacity(255);
        });

    auto seq = Sequence::create(delay, endInvincible, nullptr);
    seq->setTag(2001); 
    this->runAction(seq);
    this->setOpacity(150);
}

void Player::createRollBox()
{
    // 动态获取翻滚动画的时间，确保逻辑与表现同步
    float duration = 0.3f; // 对应 getAnimation 里的 rollStart 时间
    startRollInvincible(duration);
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
    attackBody->setCategoryBitmask(PLAYER_ATTACK);
    attackBody->setCollisionBitmask(0);            // 严禁设为 1，否则会推开怪物
    attackBody->setContactTestBitmask(ENEMY_HURT); // 只检测怪物受击

    _attackNode->setPhysicsBody(attackBody);

    _attackNode->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() { this->removeAttackBox(); }),
        nullptr
    ));
}
//盾反框
void Player::createShieldParryBox()
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