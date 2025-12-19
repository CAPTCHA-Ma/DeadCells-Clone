#include "MapUnit.h"
#include "json/rapidjson.h"
#include "json/document.h"

bool IsBoxOverLap(Box box, Box other)
{

    if (other.lowLeft.x > box.upperRight.x ||
        other.upperRight.x < box.lowLeft.x ||
        other.lowLeft.y > box.upperRight.y ||
        other.upperRight.y < box.lowLeft.y) return false;
    else return true;

}

bool IsRoomOverLap(MapUnitData* room, MapUnitData* other)
{

    if (IsBoxOverLap(room->obstacle, other->obstacle)) return true;

    return false;

}

void MapUnitData::ChangePosition()
{

    obstacle.lowLeft += velocity;
    obstacle.upperRight += velocity;

    center += velocity;
	velocity = Vec2::ZERO;

}   

static MapDataManager* _instance = nullptr;

MapDataManager* MapDataManager::getInstance()
{
    if (!_instance) _instance = new MapDataManager();
    return _instance;
}

void MapDataManager::destroyInstance()
{
    if (_instance)
    {
        delete _instance;
        _instance = nullptr;
    }
}

void parseData(const rapidjson::Value& value, std::vector<Vec2>& outData)
{

    if (!value.IsArray()) return;

    for (const auto& item : value.GetArray())
    {
        if (item.IsArray() && item.Size() >= 2)
        {
            float x = (float)item[0].GetInt();
            float y = (float)item[1].GetInt();
            outData.push_back(Vec2(x, y));
        }
    }

}

void MapDataManager::loadMapData(const std::string& jsonFile)
{
    std::string content = FileUtils::getInstance()->getStringFromFile(jsonFile);

    rapidjson::Document doc;
    doc.Parse(content.c_str());

    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
    {
        std::string fileName = it->name.GetString();
        const rapidjson::Value& value = it->value;

        RoomData roomData;
        if (value.HasMember("width"))  roomData.width = value["width"].GetInt();
        if (value.HasMember("height")) roomData.height = value["height"].GetInt();

        if (value.HasMember("entrances")) {
            parseData(value["entrances"], roomData.entrances);
        }

        if (value.HasMember("exits")) {
            parseData(value["exits"], roomData.exits);
        }

        _mapCache[fileName] = roomData;
    }
}

const RoomData* MapDataManager::getRoomData(const std::string& filename)
{
    auto it = _mapCache.find(filename);
    if (it != _mapCache.end()) return &it->second;
    return nullptr;
}