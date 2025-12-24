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

    Vec2 startDir = (rooms[0]->obstacle.lowLeft + Vec2(30, 22)) * 24;
    _player = PlayerLayer::create(startDir);
    auto _monster1 = MonsterLayer::create(MonsterCategory::Zombie, (rooms[0]->obstacle.lowLeft + Vec2(40, 22)) * 24);
    auto _monster2 = MonsterLayer::create(MonsterCategory::Grenadier, (rooms[0]->obstacle.lowLeft + Vec2(30, 22)) * 24);
    monster.pushBack(_monster1);
    monster.pushBack(_monster2);
    _mapContainer->addChild(_monster1);
    _mapContainer->addChild(_monster2);
    _mapContainer->addChild(_player);
	_mapContainer->setPosition(Director::getInstance()->getVisibleSize() / 2 - Size(startDir));

    int counter = 0;
    for (auto roomData : rooms) 
    {

        CCLOG("%d\n", ++counter);
        
        auto node = RoomNode::create(roomData);
        _mapContainer->addChild(node);

    }

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

    // 如果任何一个节点已经由于某些原因被移除，直接跳过逻辑
    if (!nodeA || !nodeB) return true;

    // ==========================================================
    // 1. 玩家近战攻击怪物 (PLAYER_ATTACK <-> ENEMY_BODY)
    // ==========================================================
    if ((maskA & PLAYER_ATTACK && maskB & ENEMY_BODY) || (maskB & PLAYER_ATTACK && maskA & ENEMY_BODY))
    {
        auto enemyNode = (maskA & ENEMY_BODY) ? nodeA : nodeB;
        // 在你的实现中，Zombie/Grenadier 本身就是绑定 Body 的 Node
        auto monster = dynamic_cast<Monster*>(enemyNode);

        if (monster && !monster->isDead())
        {
            float damage = _player->getFinalAttack();
            monster->struck(damage);
            CCLOG("Melee Hit Monster: %p | Damage: %f", monster, damage);
        }
    }

    // ==========================================================
    // 2. 玩家远程攻击怪物 (PLAYER_ARROW <-> ENEMY_BODY)
    // ==========================================================
    if ((maskA & PLAYER_ARROW && maskB & ENEMY_BODY) || (maskB & PLAYER_ARROW && maskA & ENEMY_BODY))
    {
        auto arrowNode = (maskA & PLAYER_ARROW) ? nodeA : nodeB;
        auto enemyNode = (maskA & ENEMY_BODY) ? nodeA : nodeB;

        auto arrow = dynamic_cast<Arrow*>(arrowNode);
        auto monster = dynamic_cast<Monster*>(enemyNode);

        if (arrow && !arrow->hasHit() && monster && !monster->isDead())
        {
            arrow->hit(); // 箭矢消失
            float damage = _player->getFinalAttack();
            monster->struck(damage);
            CCLOG("Arrow Hit Monster!");
        }
    }

    // ==========================================================
    // 3. 怪物攻击玩家 (ENEMY_ATTACK / ENEMY_BOMB / ENEMY_ARROW <-> PLAYER_BODY)
    // ==========================================================

    // A. 怪物近战攻击框
    if ((maskA & ENEMY_ATTACK && maskB & PLAYER_BODY) || (maskB & ENEMY_ATTACK && maskA & PLAYER_BODY))
    {
        auto attackNode = (maskA & ENEMY_ATTACK) ? nodeA : nodeB;
        if (_player && !_player->isInvincible())
        {
            // 注意：怪物的攻击框 _attackNode 是挂在 Monster 下的，所以需要 getParent
            auto monster = dynamic_cast<Monster*>(attackNode->getParent());
            float damage = monster ? monster->getFinalAttack() : 10.0f;
            _player->struck(damage);
            CCLOG("Player hit by melee!");
        }
    }

    // B. 怪物炸弹
    if ((maskA & ENEMY_BOMB && maskB & PLAYER_BODY) || (maskB & ENEMY_BOMB && maskA & PLAYER_BODY))
    {
        auto bombNode = (maskA & ENEMY_BOMB) ? nodeA : nodeB;
        auto bomb = dynamic_cast<Bomb*>(bombNode);
        if (bomb && !bomb->isExploded())
        {
            bomb->explode();
            if (_player && !_player->isInvincible()) {
                _player->struck(20.0f); // 炸弹基础伤害
            }
        }
    }

    // C. 怪物箭矢
    if ((maskA & ENEMY_ARROW && maskB & PLAYER_BODY) || (maskB & ENEMY_ARROW && maskA & PLAYER_BODY))
    {
        auto arrowNode = (maskA & ENEMY_ARROW) ? nodeA : nodeB;
        auto arrow = dynamic_cast<Arrow*>(arrowNode);
        if (arrow && !arrow->hasHit())
        {
            arrow->hit();
            if (_player && !_player->isInvincible()) {
                _player->struck(15.0f);
            }
        }
    }

    // ==========================================================
    // 4. 飞行物撞击地面 (GROUND 逻辑)
    // ==========================================================
    if ((maskA & GROUND || maskB & GROUND))
    {
        auto otherNode = (maskA & GROUND) ? nodeB : nodeA;

        // 如果撞地的是箭矢
        if (auto arrow = dynamic_cast<Arrow*>(otherNode)) {
            arrow->hit();
        }
        // 如果撞地的是炸弹
        else if (auto bomb = dynamic_cast<Bomb*>(otherNode)) {
            bomb->explode();
        }
    }

    return true;
}
