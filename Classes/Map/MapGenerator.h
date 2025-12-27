#pragma once
#include "cocos2d.h"
#include <vector>

USING_NS_CC;

struct AStarNode {
    int g = 0, f = 0, h = 0, lastDir = -1, height = 0;
    bool IsOccupied = false, extra = false;
    Vec2 last = Vec2::ZERO;
};

class MapGenerator 
{
public:

    ~MapGenerator() 
    {
    
        for (MapUnitData* room : _rooms) delete room;
        _rooms.clear();

    }
    virtual void Generate() = 0;
    virtual void GenRoomPos() = 0;
    virtual bool FindPath(std::vector<std::vector<int>>& mapArray,
        std::vector<std::vector<AStarNode>>& AStarGraph,
        Vec2 startPos, Vec2 endPos,
        std::vector<Vec2>& path) = 0;
    std::vector<class MapUnitData*>& GetRooms() { return _rooms; }

protected:
    
    void CalH(const Vec2& pos, const Vec2& endPos, std::vector<std::vector<AStarNode>>& graph) 
    {

        graph[pos.x][pos.y].h = std::abs(endPos.x - pos.x) + std::abs(endPos.y - pos.y);

    }

    bool IsOut(const Vec2& pos, const std::vector<std::vector<AStarNode>>& graph) 
    {

        if (graph.empty()) return true;

        int maxX = graph.size(), maxY = graph[0].size();
        if (pos.x < 0 || pos.x >= maxX || pos.y < 0 || pos.y >= maxY) return true;
        else return false;

    }

    struct compare 
    {

        std::vector<std::vector<AStarNode>>& graph;
        compare(std::vector<std::vector<AStarNode>>& g) : graph(g) {}
        bool operator()(const Vec2& a, const Vec2& b)
        {

            AStarNode& nodeA = graph[a.x][a.y],& nodeB = graph[b.x][b.y];

            if (nodeA.f != nodeB.f) return nodeA.f > nodeB.f;
            return nodeA.h > nodeB.h;

        }

    };

    std::vector<class MapUnitData*> _rooms;
    class MapDataManager* mDM = nullptr;

};