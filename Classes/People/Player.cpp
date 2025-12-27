#include "Player.h"
#include "Weapon.h"
#include "Shield.h"
#include "Bow.h"
#include "Sword.h"
const float pictureWidth = 250.0f;
const float pictureHeight = 250.0f;
const float bodyWidth = 100.0f;
const float bodyHeight = 150.0f;
const float runSpeed = 200.0f;
const float jumpSpeed = 500.0f;
const float rollSpeed = 500.0f;
const float climbSpeed = 100.0f;
const BasicAttributes basicAttribute = { 100,0,50 };
USING_NS_CC;
const float GRAVITY = 980.0f;
//*******************************************************************
//*******************************************************************
//*******************************************************************
//初始化a
bool Player::init()
{
    if (!Sprite::initWithFile("Graph/Player/idle_00-=-0-=-.png", Rect(0, 0, pictureWidth, pictureHeight)))
        return false;
    _runSpeed = runSpeed;
    _jumpSpeed = jumpSpeed;
    _rollSpeed = rollSpeed;
	_climbSpeed = climbSpeed;
    _state = ActionState::idle;
    _direction = MoveDirection::RIGHT;
    this->setCurrentAttributes(basicAttribute);
    this->setMaxHealth(basicAttribute.health);


    this->_mainWeapon = new Sword(Sword::SwordType::OvenAxe);
    this->_subWeapon = new Bow(Bow::BowType::dualBow);
    this->createNormalBody();


    playAnimation(ActionState::idle, true);
    this->setupHPBar();
    this->scheduleUpdate();
    return true;
}
void Player::setupHPBar()
{
    if (_hpBarNode) return;

    _hpBarNode = cocos2d::DrawNode::create();
    float spriteHeight = bodyHeight * 1;
    _hpBarNode->setPosition(cocos2d::Vec2(pictureWidth/2, spriteHeight+50));

    this->addChild(_hpBarNode, 10);
    updateHPBar();
}

void Player::updateHPBar()
{
    if (!_hpBarNode) return;
    _hpBarNode->clear();

    float width = 40.0f;  // 血条总宽度
    float height = 5.0f;  // 血条高度
    float percent = _currentAttributes.health / _maxHealth;
    percent = std::max(0.0f, std::min(1.0f, percent));
    _hpBarNode->drawSolidRect(
        cocos2d::Vec2(-width / 2, -height / 2),
        cocos2d::Vec2(width / 2, height / 2),
        cocos2d::Color4F(0, 0, 0, 0.5f)
    );
    cocos2d::Color4F barColor = (percent > 0.3f) ? cocos2d::Color4F::GREEN : cocos2d::Color4F::RED;
    _hpBarNode->drawSolidRect(
        cocos2d::Vec2(-width / 2, -height / 2),
        cocos2d::Vec2(-width / 2 + (width * percent), height / 2),
        barColor
    );
}
void Player::setupBodyProperties(cocos2d::PhysicsBody* body)
{
    body->setDynamic(true);
    body->setRotationEnable(false);
    body->setGravityEnable(true);
    body->setCategoryBitmask(PLAYER_BODY);
    body->setCollisionBitmask(GROUND | PLATFORM | LADDER | MIX | INTERACTABLE);
    body->setContactTestBitmask(ENEMY_ATTACK | ENEMY_ARROW | ENEMY_BOMB | PLATFORM | LADDER | MIX | GROUND | WEAPON | INTERACTABLE);
}
void Player::updatePhysicsBody(const cocos2d::Size& size, const cocos2d::Vec2& offset)
{
    auto currentBody = this->getPhysicsBody();
    if (!currentBody)
    {
        currentBody = PhysicsBody::create();
        this->setPhysicsBody(currentBody);
    }

    currentBody->removeAllShapes();

    float skin = 1.5f;

    float w = (size.width / 2.0f) - skin;
    float h = (size.height / 2.0f) - skin;

    if (w < 0.1f) w = 0.1f;
    if (h < 0.1f) h = 0.1f;

    float chamfer = 5.0f;

    if (chamfer > w) chamfer = w;
    if (chamfer > h) chamfer = h;

    Vec2 points[8] = {
        Vec2(-w + chamfer, -h), 
        Vec2(w - chamfer, -h),  
        Vec2(w, -h + chamfer), 
        Vec2(w, h - chamfer),  
        Vec2(w - chamfer, h), 
        Vec2(-w + chamfer, h), 
        Vec2(-w, h - chamfer), 
        Vec2(-w, -h + chamfer)  
    };

    auto material = PhysicsMaterial(0.1f, 0.0f, 0.0f);
    auto shape = PhysicsShapePolygon::create(points, 8, material, offset);

    currentBody->addShape(shape);

    setupBodyProperties(currentBody);
}
void Player::createNormalBody()
{
    updatePhysicsBody(Size(bodyWidth / 3, bodyHeight / 3), Vec2(0, bodyWidth / 4));
}
void Player::createRollBody()
{
    // 翻滚状态：高度减半，重心降低
    updatePhysicsBody(Size(bodyWidth / 3, bodyHeight / 6), Vec2(0, bodyHeight / 12));
}
void Player::giveVelocityX(float speed)
{
    int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
    this->getPhysicsBody()->setVelocity(Vec2(speed * dir, this->getPhysicsBody()->getVelocity().y));
}
void Player::giveVelocityY(float speed)
{
	int dir = (_directionY == UpDownDirection::UP) ? 1 : -1;
    this->getPhysicsBody()->setVelocity(Vec2(0, speed * dir));
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
    else if (!this->isOnGround() && _state != ActionState::rollStart)
        this->changeState(ActionState::jumpDown);

}
//*******************************************************************
//*******************************************************************
//*******************************************************************
//动作
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
    if (!this->isOnGround()) {
        return;
    }

    auto body = this->getPhysicsBody();
    if (!body)
        return;
    Vec2 impulse(0, body->getMass() * _jumpSpeed);
    body->applyImpulse(impulse);
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

void Player::hanging()
{

    auto body = this->getPhysicsBody();
    if (!body) return;

	body->setVelocity(Vec2(0, 0));
    body->setGravityEnable(false);

}

void Player::climbing()
{
    this->giveVelocityY(_climbSpeed);
}

void Player::climbedge()
{

    CCLOG("CHANGE!\n");

    auto body = this->getPhysicsBody();
    if (!body) return;

    body->setVelocity(Vec2(0, _climbSpeed));
    body->setGravityEnable(false);

}

void Player::crouch()
{
}
void Player::closeCombatBow()
{
    this->idle();
}
void Player::dualBow()
{
}
void Player::crossbowShoot()
{
}
void Player::dead()
{
    this->unscheduleUpdate();
    _invincible = true;
    this->removeAttackBox();
    this->getPhysicsBody()->setVelocity(Vec2(this->getPhysicsBody()->getVelocity().x * 0.5f, 0));
    this->changeState(ActionState::lethalFall);
}
void Player::lethalHit()
{
    this->set0VelocityX();
}
void Player::swapWeapon() // 交换主副武器
{
    auto mid = _mainWeapon;
    _mainWeapon = _subWeapon; 
    _subWeapon = mid;
    this->changeState(ActionState::idle);
    CCLOG("Swapped Main and Sub weapons");
}

void Player::getNewWeapon(Weapon* newWeapon)
{
    _mainWeapon = newWeapon;
    _animationCache.clear();
    this->changeState(ActionState::idle);
}
void Player::shootArrow()
{
    float weaponPower = 0.0f;
    if (_currentAttackingWeapon) 
    {
        weaponPower = _currentAttackingWeapon->getWeaponAttackPower();
    }
    else
    {
        weaponPower = _mainWeapon->getWeaponAttackPower();
    }
    float totalAttack = this->getCurrentAttributes().attack + weaponPower;
    int dir = (_direction == MoveDirection::RIGHT) ? 1 : -1;
    auto arrow = Arrow::create(true, totalAttack);
    if (arrow) 
    {
        arrow->setPosition(this->getPosition() + Vec2(0, 50));
        this->getParent()->addChild(arrow);
        arrow->run(Vec2(dir, 0));
    }
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
    //if (_state == ActionState::rollStart)
    //{
    //    _invincible = false;
    //    this->setOpacity(255);
    //    this->stopActionByTag(2001); // 停止无敌时间计时器
    //    this->createNormalBody();    // 强制恢复正常受击框
    //}
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
		case ActionState::hanging:this->hanging(); break;
		case ActionState::climbing:this->climbing(); break;
        case ActionState::climbedge:this->climbedge();  break;

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

        case ActionState::closeCombatBow:this->closeCombatBow(); break;
        case ActionState::dualBow:this->dualBow(); break;
        case ActionState::crossbowShoot:this->crossbowShoot(); break;

        case ActionState::blockEndLightningShield:this->createShieldParryBox(); break;
        case ActionState::blockEndParryShield:this->createShieldParryBox(); break;
        case ActionState::lethalHit:this->lethalHit(); break;
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
        if (bow->getBowType() == Bow::BowType::closeCombatBow)
            changeState(ActionState::closeCombatBow);
        else if (bow->getBowType() == Bow::BowType::dualBow)
            changeState(ActionState::dualBow);
        else if (bow->getBowType() == Bow::BowType::crossbow)
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

    if (newState == ActionState::hanging)
        return true;
    if (_state == ActionState::hanging && newState == ActionState::jumpUp)
		return true;
    if (newState == ActionState::idle)
        return true;
    if (isAttackState(_state) && isAttackState(newState))
        return true;
    if (!isOnGround() && newState == ActionState::rollStart)
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
    for (int i = 0; i < frameCount; ++i)
    {
        std::string framePath = StringUtils::format("Graph/Player/%s_%02d-=-0-=-.png", name.c_str(), i);
        auto tempSprite = Sprite::create(framePath);
        if (!tempSprite)
            continue;
        auto originalSize = tempSprite->getContentSize();
        float offsetX = (originalSize.width - pictureWidth) / 2.0f;
        float offsetY = (originalSize.height - pictureHeight) / 2.0f;
        auto frame = SpriteFrame::create(framePath, Rect(offsetX, offsetY, pictureWidth, pictureHeight));
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
        case ActionState::rollStart:        anim = createAnim("rollStart", 4, 0.3f); break;
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
        case ActionState::closeCombatBow   :anim = createAnim("closeCombatBow", 25, 0.3f); break;
        case ActionState::dualBow:       anim = createAnim("dualBow", 25, 0.3f); break;
        case ActionState::crossbowShoot:    anim = createAnim("crossbowShoot", 11, 0.3f); break;
        case ActionState::lethalHit:        anim = createAnim("lethalHit", 1, 0.25f); break;
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
        if (w == _currentAttackingWeapon) 
        {
            _comboInput = true;
        }
        return;
    }

    _currentAttackingWeapon = w;
    _comboStep = 1;
    _comboInput = false;

    this->set0VelocityX();
    this->changeStateByWeapon(w);
}

void Player::actionWhenEnding(ActionState state)
{
    if (state == ActionState::lethalFall){
        if (this->getPhysicsBody()) {
            this->getPhysicsBody()->setVelocity(Vec2::ZERO);
            this->getPhysicsBody()->setGravityEnable(false);
        }
        this->changeState(ActionState::lethalSlam);
        return;
    }
    if (state == ActionState::lethalSlam){
        gameEnding = true;
        return;
    }

    if (state == ActionState::crossbowShoot || state == ActionState::closeCombatBow || state == ActionState::dualBow){
        this->shootArrow();
    }
    //连招衔接判定
    if (_comboInput) {
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
        if (nextState != ActionState::idle) {
            _comboStep++;
            _comboInput = false;
            this->changeState(nextState);
            return;
        }
    }
    _comboStep = 0;
    _comboInput = false;
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
float Player::getFinalAttack() const
{
    float baseAtk = this->getCurrentAttributes().attack; 
    float weaponAtk = 0.0f;
    if (_currentAttackingWeapon)
    {
        weaponAtk = _currentAttackingWeapon->getWeaponAttackPower();
    }
    else
    {
        weaponAtk = 0.0f;
    }

    return baseAtk + weaponAtk;
}
void Player::struck(float attackPower)
{
    auto attributes = this->getCurrentAttributes();
    float damage = attackPower * (100 - attributes.defense) / 100;
    attributes.health -= damage;
    this->setCurrentAttributes(attributes);

    if (_currentAttributes.health <= 0) 
    {
        _currentAttributes.health = 0;
        this->dead();
    }
    else {
        this->changeState(ActionState::lethalHit);
    }

    updateHPBar(); 
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
    float duration = 0.3f; 
    startRollInvincible(duration);
}
void Player::createAttackBox()
{
    removeAttackBox();
    _attackNode = Node::create();
    float dir = (_direction == MoveDirection::RIGHT) ? 1.0f : -1.0f;
    _attackNode->setPosition(Vec2(pictureWidth / 2 + dir * pictureWidth / 6, pictureHeight * 2 / 3));
    this->addChild(_attackNode, 10);

    auto attackBody = PhysicsBody::createBox(cocos2d::Size(pictureWidth / 3, pictureHeight / 6), PhysicsMaterial(0, 0, 0));
    attackBody->setDynamic(false);
    attackBody->setCategoryBitmask(PLAYER_ATTACK);
    attackBody->setCollisionBitmask(0); 
    attackBody->setContactTestBitmask(ENEMY_BODY); 

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
    _attackNode->setPosition(Vec2(pictureWidth / 2 + dir * pictureWidth / 6, pictureHeight * 2 / 3));
    this->addChild(_attackNode, 10);

    auto attackBody = PhysicsBody::createBox(cocos2d::Size(pictureWidth / 3, pictureHeight / 6), PhysicsMaterial(0, 0, 0));
    attackBody->setDynamic(false);
    attackBody->setGravityEnable(false);
    attackBody->setCategoryBitmask(PLAYER_ATTACK);
    attackBody->setCollisionBitmask(0);
    attackBody->setContactTestBitmask(ENEMY_BODY);
    _attackNode->setPhysicsBody(attackBody);


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
