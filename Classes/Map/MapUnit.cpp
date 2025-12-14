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

    for (const auto& u : room->obstacle)
    {

        for (const auto& v : other->obstacle)
        {

            if (IsBoxOverLap(u, v)) return false;

        }

    }

    return true;

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

void parseCoordinateArray(const rapidjson::Value& jsonArray, std::vector<Vec2>& outVector)
{
    if (!jsonArray.IsArray()) return;

    for (const auto& item : jsonArray.GetArray())
    {
        if (item.IsArray() && item.Size() >= 2)
        {
            float x = (float)item[0].GetInt();
            float y = (float)item[1].GetInt();
            outVector.push_back(Vec2(x, y));
        }
    }
}

void parseDirectionData(const rapidjson::Value& value, DirectionData& outData)
{
    if (value.HasMember("top"))    parseCoordinateArray(value["top"], outData.top);
    if (value.HasMember("bottom")) parseCoordinateArray(value["bottom"], outData.bottom);
    if (value.HasMember("left"))   parseCoordinateArray(value["left"], outData.left);
    if (value.HasMember("right"))  parseCoordinateArray(value["right"], outData.right);
}

void MapDataManager::loadMapData(const std::string& jsonFile)
{
    std::string content = FileUtils::getInstance()->getStringFromFile(jsonFile);
    if (content.empty()) {
        CCLOG("MapDataManager Error: File empty or not found: %s", jsonFile.c_str());
        return;
    }

    rapidjson::Document doc;
    doc.Parse(content.c_str());

    if (doc.HasParseError()) {
        CCLOG("MapDataManager Error: JSON parse failed in %s", jsonFile.c_str());
        return;
    }

    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
    {
        std::string fileName = it->name.GetString();
        const rapidjson::Value& value = it->value;

        RoomData roomData;
        if (value.HasMember("width"))  roomData.width = value["width"].GetInt();
        if (value.HasMember("height")) roomData.height = value["height"].GetInt();

        if (value.HasMember("entrances")) {
            parseDirectionData(value["entrances"], roomData.entrances);
        }

        if (value.HasMember("exits")) {
            parseDirectionData(value["exits"], roomData.exits);
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