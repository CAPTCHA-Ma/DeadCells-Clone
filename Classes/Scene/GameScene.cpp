#include "GameScene.h"

GameScene* GameScene::createWithGenerator(MapGenerator* generator) 
{

    auto scene = new (std::nothrow) GameScene();
    if (scene && scene->initWithPhysics()) 
    {

        scene->_mapGenerator = generator;
        scene->autorelease();
        scene->init();

        // 关键：不再直接在这里运行 generate()，而是启动异步任务
        scene->startAsyncGeneration();

        scene->scheduleUpdate();
        return scene;

    }

    CC_SAFE_DELETE(scene);
    return nullptr;

}

bool GameScene::init() 
{

    // 1. 先创建一个基础的加载界面，防止黑屏
    _loadingLabel = Label::createWithSystemFont("Generating World...", "Arial", 30);
    _loadingLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
    this->addChild(_loadingLabel, 10);

    // 2. 初始化地图容器
    _mapContainer = Node::create();
    this->addChild(_mapContainer);

    return true;

}

void GameScene::startAsyncGeneration() {
    // 开启子线程进行耗时的逻辑运算
    std::thread t([this]() {
        // 这里的 generate() 包含大量的 AStarGraph 计算和循环重试逻辑
        // 只要不涉及 UI 节点的创建，在子线程运行是安全的
        this->_mapGenerator->generate();

        // 运算完成后，必须回到主线程进行渲染
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([this]() {
            this->onGenerationComplete();
            });
        });

    // 分离线程，让它独立运行
    t.detach();
}

void GameScene::onGenerationComplete() 
{

    // 移除加载提示
    if (_loadingLabel) _loadingLabel->removeFromParent();

    // 根据生成好的数据，创建视觉节点
    auto rooms = _mapGenerator->GetRooms();
    for (auto roomData : rooms) {
        // RoomNode 内部会解析 TMX 并创建物理边缘
        auto node = RoomNode::create(roomData);
        _mapContainer->addChild(node);
    }

    _player = PlayerLayer::create();
    _mapContainer->addChild(_player);

}

void GameScene::update(float dt)
{



}
