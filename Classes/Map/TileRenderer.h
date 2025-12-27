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

// Edge type
enum class EdgeType {
    NONE,
    FLOOR,
    FLOOR_LEFT,
    FLOOR_RIGHT,
    FLOOR_SINGLE,
    CEILING,
    WALL_LEFT,
    WALL_RIGHT,
    CORNER_TL,
    CORNER_TR,
    CORNER_BL,
    CORNER_BR
};

class TileRenderer : public Node {
public:
    static TileRenderer* create(const std::string& themeName);
    bool init(const std::string& themeName);
    void renderFromCollisionLayer(TMXTiledMap* tmx, const std::vector<std::vector<TileType>>& tileData);
    void setTheme(const std::string& themeName);
    
private:
    void initTexturePools();
    
    int hash(int x, int y, int salt);
    int getBlockHash(int x, int y, int blockSize, int salt);
    
    Sprite* createTileSprite(const std::string& name);
    Sprite* createNaturalSprite(const std::string& name);
    Sprite* createTileFromSheet(Texture2D* sheet, int col, int row);
    
    bool isSolid(int x, int y);
    bool isAir(int x, int y);
    bool isPlatform(int x, int y);
    bool isLadder(int x, int y);
    EdgeType getEdgeType(int x, int y);
    
    int countFloorLeft(int x, int y);
    int countFloorRight(int x, int y);
    int countWallUp(int x, int y);
    int countWallDown(int x, int y);

    void renderBackground();
    void renderTerrain();
    void renderDecorations();
    void renderForeground();
    
    void renderBackgroundDirect();
    void renderTerrainDirect();
    void renderDecorationsDirect();
    
    std::string _themeName;
    std::string _basePath;
    int _tileSize = 24;
    int _mapWidth = 0;
    int _mapHeight = 0;
    
    const std::vector<std::vector<TileType>>* _tileData = nullptr;
    
    static const int BLOCK_FLOOR = 6;
    static const int BLOCK_WALL = 8;
    static const int BLOCK_BACKGROUND = 10;
    
    std::vector<std::string> _groundTiles;
    std::vector<std::string> _groundLeftTiles;
    std::vector<std::string> _groundRightTiles;
    std::vector<std::string> _ceilingTiles;
    std::vector<std::string> _backWallTiles;
    std::vector<std::string> _stoneWallTiles;
    std::vector<std::string> _platformTiles;
    std::vector<std::string> _floorDecoTiles;
    std::vector<std::string> _wallDecoTiles;
    std::vector<std::string> _roofDecoTiles;
    std::vector<std::string> _chainTiles;
    std::vector<std::string> _railingTiles;
    
    Texture2D* _groundCellsTexture = nullptr;
    int _groundCellsCols = 22;
    int _groundCellsRows = 5;
};
