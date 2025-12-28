#pragma once
#include "cocos2d.h"
#include <vector>

USING_NS_CC;

// A*寻路所需的节点结构体
struct AStarNode {
    int g = 0, f = 0, h = 0, last_dir = -1, height = 0;
    bool is_occupied = false, extra = false;
    Vec2 last = Vec2::ZERO;
};

// 地图生成方法抽象基类
class MapGenerator 
{
public:

    virtual ~MapGenerator()
    {
    
        for (MapUnitData* room : _rooms) delete room;
        _rooms.clear();

    }
    virtual void Generate() = 0;
    virtual void GenRoomPos() = 0;
    virtual bool FindPath(std::vector<std::vector<int>>& map_array,
        std::vector<std::vector<AStarNode>>& a_star_graph,
        Vec2 start_pos, Vec2 end_pos,
        std::vector<Vec2>& path) = 0;
    std::vector<class MapUnitData*>& GetRooms() { return _rooms; }

protected:
    
	// 计算启发式函数H值（曼哈顿距离）
    void CalH(const Vec2& pos, const Vec2& endPos, std::vector<std::vector<AStarNode>>& graph) 
    {

        graph[pos.x][pos.y].h = std::abs(endPos.x - pos.x) + std::abs(endPos.y - pos.y);

    }

	// 判断位置是否越界
    bool IsOut(const Vec2& pos, const std::vector<std::vector<AStarNode>>& graph) 
    {

        if (graph.empty()) return true;

        int maxX = graph.size(), maxY = graph[0].size();
        if (pos.x < 0 || pos.x >= maxX || pos.y < 0 || pos.y >= maxY) return true;
        else return false;

    }

	// 优先队列比较器，用于A*算法
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

	// 存储房间数据的容器
    std::vector<class MapUnitData*> _rooms;
	// 管理地图信息的指针
    class MapDataManager* mDM = nullptr;

};