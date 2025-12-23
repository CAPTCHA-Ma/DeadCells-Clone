#pragma once
#include "cocos2d.h"
#include "Map/MapUnit.h"
#include "Map/MapGenerator.h"
#include "Map/RoomNode.h"
#include "People/Player.h"
#include "People/PlayerLayer.h"

class GameScene : public cocos2d::Scene
{
public:
    static GameScene* createWithGenerator(MapGenerator* generator);
    virtual bool init() override;
    virtual void update(float dt) override;

private:
    // 异步生成函数
    void startAsyncGeneration();
    // 回到主线程执行的渲染函数
    void onGenerationComplete();

    MapGenerator* _mapGenerator = nullptr;
    cocos2d::Node* _mapContainer = nullptr;
    cocos2d::Label* _loadingLabel = nullptr; // 加载提示
    PlayerLayer* _player = nullptr;
};