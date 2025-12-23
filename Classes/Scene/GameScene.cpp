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
    auto a = contact.getShapeA()->getBody();
    auto b = contact.getShapeB()->getBody();

    int ca = a->getCategoryBitmask();
    int cb = b->getCategoryBitmask();
    //玩家攻击怪物
    if ((ca == PLAYER_ATTACK && cb == ENEMY_HURT) || (ca == ENEMY_HURT && cb == PLAYER_ATTACK))
    {
        auto node = (ca == ENEMY_HURT) ? a->getNode() : b->getNode();
        if (node && node->getParent()) 
        {
            auto enemyNode = dynamic_cast<Monster*>(node->getParent());
            if (enemyNode) 
            {
                enemyNode->struck(100);
            }
        }
    }
    //怪物攻击玩家
    if ((ca == ENEMY_ATTACK && cb == PLAYER_HURT) || (ca == PLAYER_HURT && cb == ENEMY_ATTACK))
    {
        if (_player) 
        {
            _player->struck(100); 
        }
    }
    return true;
}
