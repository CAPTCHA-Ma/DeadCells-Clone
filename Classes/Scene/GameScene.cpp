#include "GameScene.h"

GameScene* GameScene::createWithGenerator(MapGenerator* generator) 
{

    auto scene = new (std::nothrow) GameScene();
    if (scene && scene->initWithPhysics()) 
    {

		scene->getPhysicsWorld()->setGravity(Vec2(0, -980.f));
		scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);  

        scene->_mapGenerator = generator;
        scene->autorelease();
        scene->init();
        scene->getPhysicsWorld()->setSubsteps(5);

        scene->GenMapData();

        return scene;

    }

    CC_SAFE_DELETE(scene);

    return nullptr;

}

bool GameScene::init() 
{

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    _loadingSprite = Sprite::create("Graph/Loading/Loading_0.png");
    auto Animation = Animation::create();

    for (int i = 0; i <= 19; i++)
    {

        std::string frameName = StringUtils::format("Graph/Loading/Loading_%d.png", i);
        Animation->addSpriteFrameWithFile(frameName);

    }

    Animation->setDelayPerUnit(0.1f);
    Animation->setRestoreOriginalFrame(true);

    auto animate = Animate::create(Animation);
    auto repeatAction = RepeatForever::create(animate);

    _loadingSprite->runAction(repeatAction);

    _loadingSprite->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height / 4 - 25));
    this->addChild(_loadingSprite);

    _loadingLabel = Label::createWithTTF(GetText("loading_text"), "fonts/fusion-pixel.ttf", 25);
    _loadingLabel->setPosition(Vec2(origin.x + visibleSize.width - 95, origin.y + visibleSize.height / 4 - 75));
    this->addChild(_loadingLabel);

    _mapContainer = Node::create();
    this->addChild(_mapContainer);

    return true;

}

void GameScene::GenMapData() 
{
    std::thread renderThread([this](){
        
        this->_mapGenerator->Generate();

        Director::getInstance()->getScheduler()->performFunctionInCocosThread([this]() {
            this->RenderMap();
            });

        });
    renderThread.detach();
}

void GameScene::RenderMap()
{

    if (_loadingLabel) _loadingLabel->removeFromParent();
    if (_loadingSprite) _loadingSprite->removeFromParent();

    auto& rooms = _mapGenerator->GetRooms();

    Vec2 startDir = (rooms[0]->obstacle.lowLeft + Vec2(30, 24)) * 24;
    _player = PlayerLayer::create(startDir);

    auto swordNode = WeaponNode::createSword(Sword::SwordType::BackStabber, (rooms[0]->obstacle.lowLeft + Vec2(40, 22)) * 24);
    _mapContainer->addChild(swordNode, 50);
    swordNode->setPrice(1000);

    auto bowNode = WeaponNode::createBow(Bow::BowType::crossbow, (rooms[0]->obstacle.lowLeft + Vec2(30, 22)) * 24);
    _mapContainer->addChild(bowNode, 50);  

    _mapContainer->addChild(_player, 100);
    _mapContainer->setPosition(Director::getInstance()->getVisibleSize() / 2 - Size(startDir));

    auto monster3 = MonsterLayer::create(MonsterCategory::Grenadier, startDir);
    _monsters.pushBack(monster3);
    _mapContainer->addChild(monster3, 100);

    int counter = 0;
    for (auto roomData : rooms)
    {

        CCLOG("%d\n", ++counter);
        auto node = RoomNode::create(roomData, this->monster);
        _mapContainer->addChild(node, 0);

    }

    auto keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    contactListener->onContactPreSolve = CC_CALLBACK_2(GameScene::onContactPreSolve, this);
    contactListener->onContactSeparate = CC_CALLBACK_1(GameScene::onContactSeparate, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    this->scheduleUpdate();

}

void GameScene::update(float dt)
{

    if (!_player || !_mapContainer) return;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    auto player = _player->getChildByName("Player");
    Vec2 playerPos = player->getPosition();

    Vec2 currentPos = _mapContainer->getPosition();
    _mapContainer->setPosition(currentPos.lerp(visibleSize / 2 - Size(playerPos), 0.1f));

    if (!_player) return;

    Vec2 playerWorldPos = _player->getPlayerWorldPosition();

    for (auto it = monster.begin(); it != monster.end(); )
    {
        MonsterLayer* mLayer = *it;
        Monster* m = mLayer->getMonster();

        if (m && m->isReadyToRemove())
        {
            mLayer->removeFromParent();
            it = monster.erase(it);     
            CCLOG("Monster Safely Deleted");
        }
        else
        {
            Vec2 playerWorldPos = _player->getPlayerWorldPosition();
            mLayer->update(dt, playerWorldPos);
            ++it;
        }
    }

}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{

    if (keyCode == EventKeyboard::KeyCode::KEY_E && _currentInteractNode != nullptr)
    {
        
        std::string name = _currentInteractNode->getName();

        if (name == "ExitDoor")
        {
            auto scene = GameOver::createScene();
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
        }
        else if (name == "Chest")
        {
            CCLOG("Open Chest!");
            // TODO: Add chest opening logic
             _currentInteractNode->removeFromParent();
        }
    }
}

bool GameScene::onContactBegin(PhysicsContact& contact)
{
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();
    int maskA = bodyA->getCategoryBitmask();
    int maskB = bodyB->getCategoryBitmask();
    auto nodeA = bodyA->getNode();
    auto nodeB = bodyB->getNode();

    if (!nodeA || !nodeB) return true;

    if ((maskA == INTERACTABLE && maskB == PLAYER_BODY) || (maskB == INTERACTABLE && maskA == PLAYER_BODY))
    {

        auto targetNode = (maskA == INTERACTABLE) ? nodeA : nodeB;
        if (targetNode) _currentInteractNode = targetNode;
        
    }

    // èŽ·å–ç¢°æ’žå‘ç”Ÿçš„å¤§è‡´ä½ç½?
    Vec2 contactPoint = contact.getContactData()->points[0];
    // ï¿½ï¿½Ò¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?
    //ï¿½ï¿½Õ½ï¿½ï¿½ï¿½ï¿½
    if ((maskA & PLAYER_ATTACK && maskB & ENEMY_BODY) || (maskB & PLAYER_ATTACK && maskA & ENEMY_BODY))
    {
        Node* enemyNode = (maskA & ENEMY_BODY) ? nodeA : nodeB;

        // ÏòÉÏ²éÕÒ Monster ÊµÀý
        Monster* monster = nullptr;
        Node* checkNode = enemyNode;
        while (checkNode != nullptr) {
            monster = dynamic_cast<Monster*>(checkNode);
            if (monster) break;
            checkNode = checkNode->getParent();
        }

        if (monster && !monster->isDead())
        {
            if (_player->isMonsterAlreadyHit(monster)) {
                return true;
            }
            _player->recordMonsterHit(monster);
            float damage = _player->getFinalAttack();
            CCLOG("New Melee Hit! Monster: %p, Damage: %f", monster, damage);
            monster->struck(damage);
        }
    }
    // ï¿½ï¿½Ò¼ï¿½Ê?
    if ((maskA & PLAYER_ARROW && maskB & ENEMY_BODY) || (maskB & PLAYER_ARROW && maskA & ENEMY_BODY))
    {
        auto arrowNode = (maskA & PLAYER_ARROW) ? nodeA : nodeB;
        auto enemyNode = (maskA & ENEMY_BODY) ? nodeA : nodeB;
        auto arrow = dynamic_cast<Arrow*>(arrowNode);
        Monster* monster = nullptr;
        Node* checkNode = enemyNode;
        while (checkNode != nullptr) {
            monster = dynamic_cast<Monster*>(checkNode);
            if (monster) break;
            checkNode = checkNode->getParent();
        }
        if (arrow && !arrow->hasHit())
        {
            arrow->hit();
            if (monster && !monster->isDead())
            {
                monster->struck(arrow->getAttackPower());
                CCLOG("Player Arrow Hit! Damage: %f", arrow->getAttackPower());
            }
        }
    }
    // ï¿½ï¿½ï¿½ï¹¥ï¿½ï¿½ï¿½ï¿½ï¿?
    // ï¿½ï¿½ï¿½ï¿½ï¿½Õ?
    if ((maskA & ENEMY_ATTACK && maskB & PLAYER_BODY) || (maskB & ENEMY_ATTACK && maskA & PLAYER_BODY))
    {
        auto attackNode = (maskA & ENEMY_ATTACK) ? nodeA : nodeB;
        auto playerNode = (maskA & PLAYER_BODY) ? nodeA : nodeB;

        // ³¢ÊÔ»ñÈ¡¹¥»÷·¢ÆðÕß£¨ÎÞÂÛÊÇ Zombie »¹ÊÇÔ¶³Ì¹ÖÎïµÄ¹¥»÷¿ò£©
        auto monster = dynamic_cast<Monster*>(attackNode->getParent());
        auto player = dynamic_cast<Player*>(playerNode);

        if (monster && player)
        {
            if (monster->hasHitTarget(player)) 
            {
                return true;
            }
            monster->recordHitTarget(player);

            if (!player->isInvincible())
            {
                player->struck(monster->getFinalAttack());
            }
        }
    }
    //ï¿½ï¿½ï¿½ï¿½Õ¨ï¿½ï¿½
    if ((maskA & ENEMY_BOMB && maskB & PLAYER_BODY) || (maskB & ENEMY_BOMB && maskA & PLAYER_BODY))
    {
        auto bombNode = (maskA & ENEMY_BOMB) ? nodeA : nodeB;
        auto bomb = dynamic_cast<Bomb*>(bombNode);
        if (bomb && !bomb->isExploded())
        {
            Vec2 sourcePos = bomb->getParent()->convertToWorldSpace(bomb->getPosition());
            float damage = bomb->getAttackPower();
            bomb->explode();
            if (_player && !_player->isInvincible())
            {
                _player->struck(damage, sourcePos);
                CCLOG("Monster Bomb Hit! Damage: %f", damage);
            }
        }
    }
    // ï¿½ï¿½ï¿½ï¿½ï¿½Ê?
    if ((maskA & ENEMY_ARROW && maskB & PLAYER_BODY) || (maskB & ENEMY_ARROW && maskA & PLAYER_BODY))
    {
        auto arrowNode = (maskA & ENEMY_ARROW) ? nodeA : nodeB;
        auto arrow = dynamic_cast<Arrow*>(arrowNode);

        if (arrow && !arrow->hasHit())
        {
            Vec2 sourcePos = arrow->getParent()->convertToWorldSpace(arrow->getPosition());
            float damage = arrow->getAttackPower();
            arrow->hit();

            if (_player && !_player->isInvincible())
            {
                _player->struck(damage, sourcePos);
                CCLOG("Enemy Arrow Hit! Damage: %f", damage);
            }
        }
    }
    //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½×² 
    if ((maskA & GROUND || maskB & GROUND))
    {
        auto otherNode = (maskA & GROUND) ? nodeB : nodeA;
        if (auto arrow = dynamic_cast<Arrow*>(otherNode))
            arrow->hit();
        else if (auto bomb = dynamic_cast<Bomb*>(otherNode)) 
            bomb->explode();
    }

    if ((maskA & WEAPON && maskB & PLAYER_BODY) || (maskA & PLAYER_BODY && maskB & WEAPON))
    {
        auto node = (maskA & WEAPON) ? nodeA : nodeB;
        _player->setNearbyWeapon(dynamic_cast<WeaponNode*>(node));
    }


    return true;
}
bool GameScene::onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve)
{
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();

    if (bodyA->getCategoryBitmask() == MIX || bodyB->getCategoryBitmask() == MIX)
    {

        PhysicsBody* playerBody = nullptr;
        PhysicsBody* mixBody = nullptr;
        if (bodyA->getCategoryBitmask() == PLAYER_BODY && bodyB->getCategoryBitmask() == MIX)
        {
            playerBody = bodyA;
            mixBody = bodyB;
        }
        else if (bodyB->getCategoryBitmask() == PLAYER_BODY && bodyA->getCategoryBitmask() == MIX)
        {
            playerBody = bodyB;
            mixBody = bodyA;
        }

        if (playerBody && mixBody)
        {

            if (playerBody->getPosition().y > mixBody->getPosition().y + 10)
            {

                if (!_player->_isBelowLadder) _player->_isAboveLadder = true;
                else _player->_isContactBottom = true;

            }
            else _player->_isAboveLadder = true;

        }

        solve.ignore();


    }

    if (bodyA->getCategoryBitmask() == GROUND || bodyB->getCategoryBitmask() == GROUND)
    {

        if (_player->getCurrentState() == ActionState::climbing || _player->getCurrentState() == ActionState::hanging)
        {

            if (_player->GetVelo().y > 0.1f && _player->_isBelowLadder) solve.ignore();
            if (_player->GetVelo().y < -0.1f && _player->_isAboveLadder) solve.ignore();

            return true;

        }

    }

    if (bodyA->getCategoryBitmask() == PLATFORM || bodyB->getCategoryBitmask() == PLATFORM)
    {

        if (_player->getCurrentState() == ActionState::climbing || _player->getCurrentState() == ActionState::hanging)
        {

            if (_player->GetVelo().y > 0.1f && _player->_isBelowLadder) solve.ignore();
            if (_player->GetVelo().y < -0.1f && _player->_isAboveLadder) solve.ignore();

            return true;

        }

        PhysicsBody* playerBody = nullptr;
        PhysicsBody* platformBody = nullptr;

        if (bodyA->getCategoryBitmask() == PLAYER_BODY && bodyB->getCategoryBitmask() == PLATFORM) {
            playerBody = bodyA;
            platformBody = bodyB;
        }
        else if (bodyB->getCategoryBitmask() == PLAYER_BODY && bodyA->getCategoryBitmask() == PLATFORM) {
            playerBody = bodyB;
            platformBody = bodyA;
        }

        if (playerBody && platformBody)
        {

            if (playerBody->getVelocity().y > 0.5)
            {

                _player->_isPassingPlatform = true;
                solve.ignore();

            }
            else if (_player->_isDropping) solve.ignore();

        }

    }

    if (bodyA->getCategoryBitmask() == LADDER || bodyB->getCategoryBitmask() == LADDER) _player->_isBelowLadder = true;

    return true;
}
void GameScene::onContactSeparate(PhysicsContact& contact)
{
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();
    int maskA = bodyA->getCategoryBitmask();
    int maskB = bodyB->getCategoryBitmask();
    auto nodeA = bodyA->getNode();
    auto nodeB = bodyB->getNode();

    WeaponNode* weapon = dynamic_cast<WeaponNode*>(nodeA);
    if (!weapon)
        weapon = dynamic_cast<WeaponNode*>(nodeB);

    if (weapon && _player)
    {
        _player->setNearbyWeapon(nullptr);
        CCLOG("Weapon out of range, pointer cleared.");
    }

    if (maskA == INTERACTABLE || maskB == INTERACTABLE)
    {

        _currentInteractNode = nullptr;

    }
    
    if (maskA & LADDER || maskB & LADDER)
    {
        _player->_isBelowLadder = false;
    }

    if (maskA & PLATFORM || maskB & PLATFORM)
    {
        _player->_isPassingPlatform = false;
    }

    if (bodyA->getCategoryBitmask() == LADDER || bodyB->getCategoryBitmask() == LADDER)
    {

        _player->_isBelowLadder = false;

    }

    if (bodyA->getCategoryBitmask() == PLATFORM || bodyB->getCategoryBitmask() == PLATFORM)
    {

        _player->_isPassingPlatform = false;

    }

    if (bodyA->getCategoryBitmask() == MIX || bodyB->getCategoryBitmask() == MIX)
    {

        PhysicsBody* playerBody = nullptr;
        PhysicsBody* mixBody = nullptr;
        if (bodyA->getCategoryBitmask() == PLAYER_BODY && bodyB->getCategoryBitmask() == MIX)
        {
            playerBody = bodyA;
            mixBody = bodyB;
        }
        else if (bodyB->getCategoryBitmask() == PLAYER_BODY && bodyA->getCategoryBitmask() == MIX)
        {
            playerBody = bodyB;
            mixBody = bodyA;
        }
        if (playerBody && mixBody)
        {

            _player->_isContactBottom = false;
            _player->_isAboveLadder = false;

        }

    }

}

GameScene::~GameScene()
{

    if (_mapGenerator)
    {
        delete _mapGenerator;
        _mapGenerator = nullptr;
    }

    Director::getInstance()->getTextureCache()->removeUnusedTextures();

}
