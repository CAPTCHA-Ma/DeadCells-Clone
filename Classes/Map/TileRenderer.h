#pragma once
#include "cocos2d.h"
#include <vector>
#include <string>
#include <unordered_map>

USING_NS_CC;

enum class TileType {
    AIR,
    GROUND,
    PLATFORM,
    LADDER
};

// 边缘类型
enum class EdgeType {
    NONE,           // 内部块（不渲染）
    FLOOR,          // 地板表面
    FLOOR_LEFT,     // 地板左边缘
    FLOOR_RIGHT,    // 地板右边缘
    FLOOR_SINGLE,   // 独立单格地板
    CEILING,        // 天花板
    WALL_LEFT,      // 左墙
    WALL_RIGHT,     // 右墙
    CORNER_TL,      // 左上角
    CORNER_TR,      // 右上角
    CORNER_BL,      // 左下角
    CORNER_BR       // 右下角
};

class TileRenderer : public Node {
public:
    static TileRenderer* create(const std::string& themeName);
    bool init(const std::string& themeName);
    void renderFromCollisionLayer(TMXTiledMap* tmx, const std::vector<std::vector<TileType>>& tileData);
    void setTheme(const std::string& themeName);
    
private:
    void initTexturePools();
    
    // Hash函数
    int hash(int x, int y, int salt);
    int getBlockHash(int x, int y, int blockSize, int salt);  // 区块化hash
    
    // 精灵创建
    Sprite* createTileSprite(const std::string& name);      // 强制24x24
    Sprite* createNaturalSprite(const std::string& name);   // 原始尺寸
    Sprite* createTileFromSheet(Texture2D* sheet, int col, int row);  // 从tileset切割
    
    // 辅助函数
    bool isSolid(int x, int y);
    bool isAir(int x, int y);
    bool isPlatform(int x, int y);
    bool isLadder(int x, int y);
    EdgeType getEdgeType(int x, int y);
    
    // 检测连续段
    int countFloorLeft(int x, int y);   // 左边连续地板数
    int countFloorRight(int x, int y);  // 右边连续地板数
    int countWallUp(int x, int y);      // 上方连续墙壁数
    int countWallDown(int x, int y);    // 下方连续墙壁数

    // 渲染层 (RenderTexture版本)
    void renderBackground();      // 背景墙
    void renderTerrain();         // 基础地形
    void renderDecorations();     // 装饰物
    void renderForeground();      // 前景
    
    // 直接渲染版本 (直接添加sprite到节点)
    void renderBackgroundDirect();
    void renderTerrainDirect();
    void renderDecorationsDirect();
    
    // 数据
    std::string _themeName;
    std::string _basePath;
    int _tileSize = 24;
    int _mapWidth = 0;
    int _mapHeight = 0;
    
    const std::vector<std::vector<TileType>>* _tileData = nullptr;
    
    // 区块尺寸配置
    static const int BLOCK_FLOOR = 6;      // 地板区块 6x6
    static const int BLOCK_WALL = 8;       // 墙壁区块 8x8
    static const int BLOCK_BACKGROUND = 10; // 背景区块 10x10
    
    // 贴图池（简化，只保留2种变体）
    std::vector<std::string> _groundTiles;      // 地板中间
    std::vector<std::string> _groundLeftTiles;  // 地板左边缘
    std::vector<std::string> _groundRightTiles; // 地板右边缘
    std::vector<std::string> _ceilingTiles;     // 天花板
    std::vector<std::string> _backWallTiles;    // 背景墙
    std::vector<std::string> _stoneWallTiles;   // 石墙(72x24)
    std::vector<std::string> _platformTiles;    // 平台
    std::vector<std::string> _floorDecoTiles;   // 地面装饰
    std::vector<std::string> _wallDecoTiles;    // 墙壁装饰
    std::vector<std::string> _roofDecoTiles;    // 天花板装饰
    std::vector<std::string> _chainTiles;       // 链条
    std::vector<std::string> _railingTiles;     // 栏杆
    
    // 地形Tileset
    Texture2D* _groundCellsTexture = nullptr;
    int _groundCellsCols = 22;  // 528/24
    int _groundCellsRows = 5;   // 120/24
};
