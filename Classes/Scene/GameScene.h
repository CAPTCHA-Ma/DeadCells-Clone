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
    Vector<MonsterLayer*> monster;
    MapGenerator* _mapGenerator = nullptr;
    cocos2d::Node* _mapContainer = nullptr;
    cocos2d::Label* _loadingLabel = nullptr; 
	cocos2d::Sprite* _loadingSprite = nullptr;
    PlayerLayer* _player = nullptr;
	cocos2d::Vector<MonsterLayer*> _monsters;
    cocos2d::Vector<WeaponNode*> _weapons;
private:
    bool onContactBegin(cocos2d::PhysicsContact& contact);
    bool onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve);
    void onContactSeparate(cocos2d::PhysicsContact& contact);
    void handlePlayerWeaponContact(cocos2d::Node* nodeA, cocos2d::Node* nodeB, bool isBegin);
    void handleLadderAndPlatformLogic(cocos2d::PhysicsBody* a, cocos2d::PhysicsBody* b, cocos2d::PhysicsContactPreSolve& solve);
};