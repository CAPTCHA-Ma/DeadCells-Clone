#pragma once
#include "cocos2d.h"
#include "Map/MapUnit.h"
#include "Map/MapGenerator.h"
#include "Map/RoomNode.h"
#include "People/PlayerLayer.h"
#include "People/MonsterLayer.h"
#include "People/Zombie.h"
#include "Res/strings.h"
#include "People/Grenadier.h"
#include "People/Monster.h"
#include "GameOver.h"

// 游戏场景类，负责地图生成、渲染和游戏逻辑处理
class GameScene : public cocos2d::Scene
{
public:
    static GameScene* createWithGenerator(MapGenerator* generator);
    virtual bool init() override;
    virtual void update(float dt) override;
	~GameScene();
private:
    void GenMapData();
    void RenderMap();
    bool onContactBegin(PhysicsContact& contact);
    bool onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve);
    void onContactSeparate(PhysicsContact& contact);
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    Vector<MonsterLayer*> monster;
    MapGenerator* _mapGenerator = nullptr;
    cocos2d::Node* _mapContainer = nullptr;
    cocos2d::Label* _loadingLabel = nullptr; 
	cocos2d::Sprite* _loadingSprite = nullptr;
    PlayerLayer* _player = nullptr;
    cocos2d::Node* _currentInteractNode = nullptr;

};