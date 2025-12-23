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

        // 如果怪物已经标记为“准备移除”
        if (m && m->isReadyToRemove())
        {
            mLayer->removeFromParent(); // 从渲染树移除
            it = monster.erase(it);      // 从 Vector 移除，指针不再是 0xDDDDDDDD
            CCLOG("Monster Safely Deleted");
        }
        else
        {
            // 只有没死或动画还没播完的怪物才更新 AI
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

    // --- 逻辑 A：玩家打怪物 ---
    if ((maskA == PLAYER_ATTACK && maskB == ENEMY_HURT) || (maskA == ENEMY_HURT && maskB == PLAYER_ATTACK))
    {
        auto hurtBody = (maskA == ENEMY_HURT) ? bodyA : bodyB;
        auto hurtNode = hurtBody->getNode();
        if (hurtNode && hurtNode->getParent())
        {
            auto monster = dynamic_cast<Monster*>(hurtNode->getParent());
            if (monster && !monster->isDead())
            {
                // 使用玩家的攻击力
                float damage = _player->getFinalAttack();
                monster->struck(damage);
                CCLOG("Monster Hit!");
            }
        }
    }

    // --- 逻辑 B：怪物打玩家 ---
    if ((maskA == ENEMY_ATTACK && maskB == PLAYER_HURT) || (maskA == PLAYER_HURT && maskB == ENEMY_ATTACK))
    {
        // 1. 找到攻击发起者
        auto attackBody = (maskA == ENEMY_ATTACK) ? bodyA : bodyB;
        auto attackNode = attackBody->getNode();

        if (attackNode && attackNode->getParent() && _player && !_player->isInvincible())
        {
            // 2. 溯源到 Monster 脚本
            auto monster = dynamic_cast<Monster*>(attackNode->getParent());
            if (monster)
            {
                // 3. 动态获取怪物攻击力并扣血
                float damage = monster->getFinalAttack();
                _player->struck(damage);
                CCLOG("Player struck by monster! Damage: %f", damage);
            }
        }
    }
    //检测 炸弹(ENEMY_BOMB) 与 玩家受击框(PLAYER_HURT) 的碰撞
    if ((maskA == ENEMY_BOMB && maskB == PLAYER_HURT) || (maskB == ENEMY_BOMB && maskA == PLAYER_HURT)) 
    {

        // 1. 找到炸弹节点并触发爆炸
        auto bombNode = (maskA == ENEMY_BOMB) ? dynamic_cast<Bomb*>(bodyA->getNode()) : dynamic_cast<Bomb*>(bodyB->getNode());

        if (bombNode && !bombNode->isExploded()) 
        {
            bombNode->explode(); // 执行爆炸动画

            // 2. 玩家受伤逻辑
            if (_player && !_player->isInvincible())
                _player->struck(20.0f); // 伤害数值
        }
    }
    if ((maskA == PLAYER_ARROW && maskB == ENEMY_HURT) || (maskA == ENEMY_HURT && maskB == PLAYER_ARROW)) 
    {
        auto arrowNode = dynamic_cast<Arrow*>((maskA == PLAYER_ARROW) ? bodyA->getNode() : bodyB->getNode());
        auto enemyNode = (maskA == ENEMY_HURT) ? bodyA->getNode() : bodyB->getNode();

        if (arrowNode && !arrowNode->hasHit())
        {
            arrowNode->hit(); // 销毁箭
            if (enemyNode && enemyNode->getParent()) 
            {
                auto monster = dynamic_cast<Monster*>(enemyNode->getParent());
                float damage = _player->getFinalAttack();
                monster->struck(damage);
            }
        }
    }

    //怪物的箭 命中 玩家
    if ((maskA == ENEMY_ARROW && maskB == PLAYER_HURT) || (maskA == PLAYER_HURT && maskB == ENEMY_ARROW)) 
    {
        auto arrowNode = dynamic_cast<Arrow*>((maskA == ENEMY_ARROW) ? bodyA->getNode() : bodyB->getNode());
        if (arrowNode && !arrowNode->hasHit() && _player && !_player->isInvincible()) 
        {
            arrowNode->hit();
            _player->struck(15.0f); 
        }
    }

    // 炸弹撞地逻辑
    if ((maskA == ENEMY_BOMB && maskB == GROUND) || (maskA == GROUND && maskB == ENEMY_BOMB)) 
    {
        auto bombNode = (maskA == ENEMY_BOMB) ? contact.getShapeA()->getBody()->getNode() : contact.getShapeB()->getBody()->getNode();
        auto bomb = dynamic_cast<Bomb*>(bombNode);
        if (bomb && !bomb->isExploded()) 
        {
            bomb->explode();
        }
    }

    // 箭矢撞地逻辑
    if ((maskA == PLAYER_ARROW && maskB == GROUND) || (maskA == GROUND && maskB == PLAYER_ARROW)) 
    {
        auto arrowNode = (maskA == PLAYER_ARROW) ? contact.getShapeA()->getBody()->getNode() : contact.getShapeB()->getBody()->getNode();
        auto arrow = dynamic_cast<Arrow*>(arrowNode);
        if (arrow) 
            arrow->hit();
    }

    return true;
    return true;
}
