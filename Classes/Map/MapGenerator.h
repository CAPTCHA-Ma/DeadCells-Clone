#pragma once
#include "cocos2d.h"
#include <vector>

USING_NS_CC;

// 定义 AStarNode
struct AStarNode {
    int g = 0, f = 0, h = 0, lastDir = -1, height = 0;
    bool IsOccupied = false, extra = false;
    Vec2 last = Vec2::ZERO;
};

// 使用别名简化代码
using AStarGrid = std::vector<std::vector<AStarNode>>;

class MapGenerator 
{
public:
    virtual ~MapGenerator() {}
    virtual void generate() = 0;
    virtual void GenRoomPos() = 0;
    virtual bool FindPath(std::vector<std::vector<int>>& mapArray,
        std::vector<std::vector<AStarNode>>& AStarGraph,
        Vec2 startPos, Vec2 endPos,
        std::vector<Vec2>& path) = 0;
    std::vector<class MapUnitData*>& GetRooms() {
        return _rooms;
	}

protected:
    // 将工具函数封装进基类，方便子类调用
    void CalH(const Vec2& pos, const Vec2& endPos, AStarGrid& graph) {
        graph[pos.x][pos.y].h = std::abs(endPos.x - pos.x) + std::abs(endPos.y - pos.y);
    }

    bool IsOut(const Vec2& pos, const AStarGrid& graph) {
        if (graph.empty()) return true;
        int maxX = graph.size();
        int maxY = graph[0].size();
        return (pos.x < 0 || pos.x >= maxX || pos.y < 0 || pos.y >= maxY);
    }

    // 比较器：通过构造函数传入当前正在使用的图引用
    struct NodeComparator {
        const AStarGrid& graph;
        NodeComparator(const AStarGrid& g) : graph(g) {}
        bool operator()(const Vec2& a, const Vec2& b) const {
            const auto& nodeA = graph[a.x][a.y];
            const auto& nodeB = graph[b.x][b.y];
            if (nodeA.f != nodeB.f) return nodeA.f > nodeB.f;
            return nodeA.h > nodeB.h;
        }
    };

    std::vector<class MapUnitData*> _rooms;
    class MapDataManager* mDM = nullptr;
};