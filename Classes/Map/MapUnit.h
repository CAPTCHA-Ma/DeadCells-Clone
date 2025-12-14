#pragma once
#include "cocos2d.h"
#include "People/Monster.h"
#include "People/Weapon.h"
#include <vector>
#include <map>
#include <string>
USING_NS_CC;

enum class Type
{

    elite, treasure, trader, exit, combat, corridor, start

};

enum class Direction
{

    top, bottom, left, right

};

struct Box
{

    Vec2 lowLeft, upperRight;

};

struct door
{

	Direction dir;
	Vec2 pos;

};

class MapUnitData
{
public:

	Type roomtype;
	std::string name;
    std::vector<door> entrance, exit;
	std::vector<Box> obstacle;
	MapUnitData* preRoom = nullptr;
    std::vector<MapUnitData*> nextRoom;

};

class MapUnit : public cocos2d::Node
{



};

struct DirectionData
{

    std::vector<Vec2> top, bottom, left, right;

};

struct RoomData
{

    int width = 0, height = 0;
    DirectionData entrances, exits;     

};

class MapDataManager
{

public:

    static MapDataManager* getInstance();
    static void destroyInstance();

    void loadMapData(const std::string& jsonFile);
    const RoomData* getRoomData(const std::string& filename);

private:

    std::map<std::string, RoomData> _mapCache;

};

bool IsBoxOverLap(Box box, Box other);
bool IsRoomOverLap(MapUnitData* room, MapUnitData* other);