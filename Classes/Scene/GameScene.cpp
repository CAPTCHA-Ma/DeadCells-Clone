#include "GameScene.h"

GameScene* GameScene::createWithGenerator(MapGenerator* generator) 
{

    auto scene = new (std::nothrow) GameScene();
    if (scene && scene->initWithPhysics()) 
    {

		scene->getPhysicsWorld()->setGravity(Vec2(0, -980.f));
		// scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);  // 鍏抽棴璋冭瘯缁樺埗

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


    auto listener = EventListenerPhysicsContact::create();
    listener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


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

    auto rooms = _mapGenerator->GetRooms();

    Vec2 startDir = (rooms[0]->obstacle.lowLeft + Vec2(30, 24)) * 24;
    _player = PlayerLayer::create(startDir);


    auto swordNode = WeaponNode::createSword(Sword::SwordType::BackStabber, (rooms[0]->obstacle.lowLeft + Vec2(40, 22)) * 24);
    _mapContainer->addChild(swordNode);

    auto bowNode = WeaponNode::createBow(Bow::BowType::crossbow, (rooms[0]->obstacle.lowLeft + Vec2(30, 22)) * 24);
    _mapContainer->addChild(bowNode);


   /* auto _monster1 = MonsterLayer::create(MonsterCategory::Zombie, (rooms[0]->obstacle.lowLeft + Vec2(40, 22)) * 24);
    auto _monster2 = MonsterLayer::create(MonsterCategory::Grenadier, (rooms[0]->obstacle.lowLeft + Vec2(30, 22)) * 24);
    monster.pushBack(_monster1);
    monster.pushBack(_monster2);
    _mapContainer->addChild(_monster1);
    _mapContainer->addChild(_monster2);*/
    _mapContainer->addChild(_player);
	_mapContainer->setPosition(Director::getInstance()->getVisibleSize() / 2 - Size(startDir));

	auto monster3 = MonsterLayer::create(MonsterCategory::Grenadier, startDir);
	_monsters.pushBack(monster3);
	_mapContainer->addChild(monster3);

    int counter = 0;
    for (auto roomData : rooms) 
    {

        CCLOG("%d\n", ++counter);
        auto node = RoomNode::create(roomData, this->monster);
        _mapContainer->addChild(node);

    }

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactPreSolve = [this](PhysicsContact& contact, PhysicsContactPreSolve& solve) 
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

        };

        contactListener->onContactSeparate = [this](PhysicsContact& contact) {
            auto bodyA = contact.getShapeA()->getBody();
            auto bodyB = contact.getShapeB()->getBody();

            if (bodyA->getCategoryBitmask() == LADDER || bodyB->getCategoryBitmask() == LADDER) 
            {

                //CCLOG("NOTBELOWLADDER!\n");
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

            };
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
            mLayer->removeFromParent(); // 从渲染树移除
            it = monster.erase(it);      // 从 Vector 移除
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

bool GameScene::onContactBegin(PhysicsContact& contact)
{
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();

    int maskA = bodyA->getCategoryBitmask();
    int maskB = bodyB->getCategoryBitmask();

    auto nodeA = bodyA->getNode();
    auto nodeB = bodyB->getNode();

    if (!nodeA || !nodeB) return true;

    // 获取碰撞发生的大致位置
    Vec2 contactPoint = contact.getContactData()->points[0];

    // ==========================================================
    // 玩家攻击怪物
    // ==========================================================

    //近战攻击
    if ((maskA & PLAYER_ATTACK && maskB & ENEMY_BODY) || (maskB & PLAYER_ATTACK && maskA & ENEMY_BODY))
    {
        auto enemyNode = (maskA & ENEMY_BODY) ? nodeA : nodeB;
        auto monster = dynamic_cast<Monster*>(enemyNode);
        if (monster && !monster->isDead())
        {
            monster->struck(_player->getFinalAttack());
        }
    }

    // 玩家箭矢
    if ((maskA & PLAYER_ARROW && maskB & ENEMY_BODY) || (maskB & PLAYER_ARROW && maskA & ENEMY_BODY))
    {
        auto arrowNode = (maskA & PLAYER_ARROW) ? nodeA : nodeB;
        auto enemyNode = (maskA & ENEMY_BODY) ? nodeA : nodeB;
        auto arrow = dynamic_cast<Arrow*>(arrowNode);
        auto monster = dynamic_cast<Monster*>(enemyNode);

        if (arrow && !arrow->hasHit())
        {
            arrow->hit();
            if (monster && !monster->isDead()) 
            {
                monster->struck(_player->getFinalAttack());
            }
        }
    }

    // ==========================================================
    // 怪物攻击玩家
    // ==========================================================

    // 怪物近战
    if ((maskA & ENEMY_ATTACK && maskB & PLAYER_BODY) || (maskB & ENEMY_ATTACK && maskA & PLAYER_BODY))
    {
        auto attackNode = (maskA & ENEMY_ATTACK) ? nodeA : nodeB;
        if (_player && !_player->isInvincible())
        {
            auto monster = dynamic_cast<Monster*>(attackNode->getParent());
            float damage = monster ? monster->getFinalAttack() : 10.0f;


            Vec2 sourcePos = attackNode->getParent()->convertToWorldSpace(attackNode->getPosition());
            _player->struck(damage, sourcePos); 
        }
    }

    //怪物炸弹
    if ((maskA & ENEMY_BOMB && maskB & PLAYER_BODY) || (maskB & ENEMY_BOMB && maskA & PLAYER_BODY))
    {
        auto bombNode = (maskA & ENEMY_BOMB) ? nodeA : nodeB;
        auto bomb = dynamic_cast<Bomb*>(bombNode);
        if (bomb && !bomb->isExploded())
        {
            Vec2 sourcePos = bomb->getParent()->convertToWorldSpace(bomb->getPosition());
            bomb->explode();
            if (_player && !_player->isInvincible())
            {
                _player->struck(20.0f, sourcePos);
            }
        }
    }

    // 怪物箭矢
    if ((maskA & ENEMY_ARROW && maskB & PLAYER_BODY) || (maskB & ENEMY_ARROW && maskA & PLAYER_BODY))
    {
        auto arrowNode = (maskA & ENEMY_ARROW) ? nodeA : nodeB;
        auto arrow = dynamic_cast<Arrow*>(arrowNode);
        if (arrow && !arrow->hasHit())
        {
            Vec2 sourcePos = arrow->getParent()->convertToWorldSpace(arrow->getPosition());
            arrow->hit();
            if (_player && !_player->isInvincible())
            {
                _player->struck(15.0f, sourcePos);
            }
        }
    }

    // ==========================================================
    //环境碰撞 
    // ==========================================================
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
        _player->_nearbyWeapon = dynamic_cast<WeaponNode*>(node);
    }


    return true;
}
void GameScene::onContactSeparate(PhysicsContact& contact)
{
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();
    int maskA = bodyA->getCategoryBitmask();
    int maskB = bodyB->getCategoryBitmask();

    // 当玩家离开武器的感应范围
    if ((maskA & WEAPON && maskB & PLAYER_BODY) || (maskB & WEAPON && maskA & PLAYER_BODY))
    {
        if (_player) 
            _player->_nearbyWeapon = nullptr; // 取消标记，防止隔空捡取
    }
}
