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

class GameScene : public cocos2d::Scene
{
public:
    static GameScene* createWithGenerator(MapGenerator* generator);
    virtual bool init() override;
    virtual void update(float dt) override;

private:

    void GenMapData();
    void RenderMap();
    bool onContactBegin(PhysicsContact& contact);

    Vector<MonsterLayer*> monster;
    MapGenerator* _mapGenerator = nullptr;
    cocos2d::Node* _mapContainer = nullptr;
    cocos2d::Label* _loadingLabel = nullptr; 
	cocos2d::Sprite* _loadingSprite = nullptr;
    PlayerLayer* _player = nullptr;
	cocos2d::Vector<MonsterLayer*> _monsters;

};