#include "TileRenderer.h"

// Z层级
const int Z_BACKGROUND = -200;
const int Z_TERRAIN = 0;
const int Z_DECORATION = 50;
const int Z_FOREGROUND = 200;

TileRenderer* TileRenderer::create(const std::string& themeName) {
    auto renderer = new TileRenderer();
    if (renderer && renderer->init(themeName)) {
        renderer->autorelease();
        return renderer;
    }
    CC_SAFE_DELETE(renderer);
    return nullptr;
}

bool TileRenderer::init(const std::string& themeName) {
    if (!Node::init()) return false;
    _themeName = themeName;
    _basePath = themeName + "/";
    initTexturePools();
    
    // 加载groundCells tileset
    std::string groundCellsPath = _basePath + "tiles/groundCells-=-0-=-.png";
    _groundCellsTexture = Director::getInstance()->getTextureCache()->addImage(groundCellsPath);
    if (_groundCellsTexture) {
        _groundCellsTexture->retain();
    }
    
    return true;
}

void TileRenderer::setTheme(const std::string& themeName) {
    _themeName = themeName;
    _basePath = themeName + "/";
}

void TileRenderer::initTexturePools() {
    // === 使用新的目录结构 ===
    // 地板 (tiles/)
    _groundTiles = {"tiles/ground_0", "tiles/ground_1", "tiles/ground_2", "tiles/ground_3"};
    _groundLeftTiles = {"tiles/ground_left_0", "tiles/ground_left_1"};
    _groundRightTiles = {"tiles/ground_right_0", "tiles/ground_right_1"};
    
    // 天花板 (tiles/)
    _ceilingTiles = {"tiles/ceiling_0", "tiles/ceiling_1", "tiles/ceiling_2", "tiles/ceiling_3"};
    
    // 背景墙 (tiles/)
    _backWallTiles = {
        "tiles/backWall_0", "tiles/backWall_1", "tiles/backWall_2",
        "tiles/backWall_3", "tiles/backWall_4", "tiles/backWall_5"
    };
    
    // 石墙 (tiles/)
    _stoneWallTiles = {"tiles/stoneWall_0", "tiles/stoneWall_1", "tiles/stoneWall_2", "tiles/stoneWall_3"};
    
    // 平台 (platforms/)
    _platformTiles = {
        "platforms/woodenPlatform_0", "platforms/woodenPlatform_1",
        "platforms/woodenPlatform_2", "platforms/woodenPlatform_3", "platforms/woodenPlatform_4"
    };

    // 地面装饰 (floor_decorations/)
    _floorDecoTiles = {
        "floor_decorations/floorjunk_0", "floor_decorations/floorjunk_1",
        "floor_decorations/floorjunk_2", "floor_decorations/floorjunk_3",
        "floor_decorations/floorjunk_4", "floor_decorations/floorjunk_5",
        "plants/leaf_0", "plants/leaf_1", "plants/leaf_2"
    };
    
    // 墙壁装饰 (wall_decorations/)
    _wallDecoTiles = {
        "wall_decorations/wallJunk_0", "wall_decorations/wallJunk_1",
        "wall_decorations/wallJunk_2", "wall_decorations/wallJunk_3",
        "wall_decorations/stampWallSmall_0", "wall_decorations/stampWallSmall_1"
    };
    
    // 天花板装饰 (plants/ + wall_decorations/)
    _roofDecoTiles = {
        "plants/roofMoss_0", "plants/roofMoss_1", "plants/roofMoss_2",
        "wall_decorations/stampCeilingSmall_0", "wall_decorations/stampCeilingSmall_1"
    };
    
    // 链条 (chains/)
    _chainTiles = {"chains/chainTile"};
    
    // 栏杆 (npcs/) - 原来的railing贴图
    _railingTiles = {"npcs/railing_0", "npcs/railing_1", "npcs/railing_2", "npcs/railing_3"};
}

// =====================================================
// Hash 函数
// =====================================================
int TileRenderer::hash(int x, int y, int salt) {
    int h = x * 374761393 + y * 668265263 + salt;
    h = (h ^ (h >> 13)) * 1274126177;
    return abs(h ^ (h >> 16));
}

// 区块化hash：同一区块内返回相同值
int TileRenderer::getBlockHash(int x, int y, int blockSize, int salt) {
    int bx = x / blockSize;
    int by = y / blockSize;
    return hash(bx, by, salt);
}

// =====================================================
// 精灵创建
// =====================================================
Sprite* TileRenderer::createTileSprite(const std::string& name) {
    if (name.empty()) return nullptr;
    // 支持带子目录的路径
    std::string path = _basePath + name + "-=-0-=-.png";
    auto sprite = Sprite::create(path);
    if (!sprite) {
        // 尝试不带子目录的路径（兼容旧代码）
        path = _basePath + name.substr(name.find_last_of('/') + 1) + "-=-0-=-.png";
        sprite = Sprite::create(path);
    }
    if (sprite) {
        sprite->setContentSize(Size(_tileSize, _tileSize));
    }
    return sprite;
}

Sprite* TileRenderer::createNaturalSprite(const std::string& name) {
    if (name.empty()) return nullptr;
    // 支持带子目录的路径
    std::string path = _basePath + name + "-=-0-=-.png";
    auto sprite = Sprite::create(path);
    if (!sprite) {
        // 尝试不带子目录的路径（兼容旧代码）
        path = _basePath + name.substr(name.find_last_of('/') + 1) + "-=-0-=-.png";
        sprite = Sprite::create(path);
    }
    return sprite;
}

Sprite* TileRenderer::createTileFromSheet(Texture2D* sheet, int col, int row) {
    if (!sheet) return nullptr;
    
    // 从tileset中切割指定位置的tile
    Rect rect(col * _tileSize, row * _tileSize, _tileSize, _tileSize);
    auto sprite = Sprite::createWithTexture(sheet, rect);
    return sprite;
}

// =====================================================
// 辅助函数
// =====================================================
bool TileRenderer::isSolid(int x, int y) {
    // 边界外视为实心（这样边缘格子才会被识别为边缘）
    if (!_tileData || y < 0 || y >= _mapHeight || x < 0 || x >= _mapWidth) return true;
    return (*_tileData)[y][x] == TileType::GROUND;
}

bool TileRenderer::isAir(int x, int y) {
    if (!_tileData || y < 0 || y >= _mapHeight || x < 0 || x >= _mapWidth) return false;
    return (*_tileData)[y][x] == TileType::AIR;
}

bool TileRenderer::isPlatform(int x, int y) {
    if (!_tileData || y < 0 || y >= _mapHeight || x < 0 || x >= _mapWidth) return false;
    return (*_tileData)[y][x] == TileType::PLATFORM;
}

bool TileRenderer::isLadder(int x, int y) {
    if (!_tileData || y < 0 || y >= _mapHeight || x < 0 || x >= _mapWidth) return false;
    return (*_tileData)[y][x] == TileType::LADDER;
}

// 左边连续地板数
int TileRenderer::countFloorLeft(int x, int y) {
    int count = 0;
    for (int i = x - 1; i >= 0; i--) {
        if (isSolid(i, y) && !isSolid(i, y + 1)) count++;
        else break;
    }
    return count;
}

// 右边连续地板数
int TileRenderer::countFloorRight(int x, int y) {
    int count = 0;
    for (int i = x + 1; i < _mapWidth; i++) {
        if (isSolid(i, y) && !isSolid(i, y + 1)) count++;
        else break;
    }
    return count;
}

// 上方连续墙壁数
int TileRenderer::countWallUp(int x, int y) {
    bool isLeft = !isSolid(x - 1, y);
    int count = 0;
    for (int i = y + 1; i < _mapHeight; i++) {
        if (!isSolid(x, i)) break;
        bool stillWall = isLeft ? !isSolid(x - 1, i) : !isSolid(x + 1, i);
        if (!stillWall) break;
        count++;
    }
    return count;
}

// 下方连续墙壁数
int TileRenderer::countWallDown(int x, int y) {
    bool isLeft = !isSolid(x - 1, y);
    int count = 0;
    for (int i = y - 1; i >= 0; i--) {
        if (!isSolid(x, i)) break;
        bool stillWall = isLeft ? !isSolid(x - 1, i) : !isSolid(x + 1, i);
        if (!stillWall) break;
        count++;
    }
    return count;
}

// 获取边缘类型（简化版）
EdgeType TileRenderer::getEdgeType(int x, int y) {
    if (!_tileData) return EdgeType::NONE;
    if (y < 0 || y >= _mapHeight || x < 0 || x >= _mapWidth) return EdgeType::NONE;
    if ((*_tileData)[y][x] != TileType::GROUND) return EdgeType::NONE;
    
    // 检查四个方向
    auto getType = [&](int tx, int ty) -> TileType {
        if (ty < 0 || ty >= _mapHeight || tx < 0 || tx >= _mapWidth) return TileType::AIR;
        return (*_tileData)[ty][tx];
    };
    
    bool solidAbove = (getType(x, y + 1) == TileType::GROUND);
    bool solidBelow = (getType(x, y - 1) == TileType::GROUND);
    bool solidLeft = (getType(x - 1, y) == TileType::GROUND);
    bool solidRight = (getType(x + 1, y) == TileType::GROUND);
    
    // 完全被实心包围 = 内部块
    if (solidAbove && solidBelow && solidLeft && solidRight) {
        return EdgeType::NONE;
    }
    
    // 地板（上方不是实心）
    if (!solidAbove) {
        if (!solidLeft && !solidRight) return EdgeType::FLOOR_SINGLE;
        if (!solidLeft) return EdgeType::FLOOR_LEFT;
        if (!solidRight) return EdgeType::FLOOR_RIGHT;
        return EdgeType::FLOOR;
    }
    
    // 天花板（下方不是实心）
    if (!solidBelow) {
        return EdgeType::CEILING;
    }
    
    // 墙壁
    if (!solidLeft) return EdgeType::WALL_LEFT;
    if (!solidRight) return EdgeType::WALL_RIGHT;
    
    return EdgeType::FLOOR;  // 默认返回地板
}

// =====================================================
// 主渲染入口
// =====================================================
void TileRenderer::renderFromCollisionLayer(TMXTiledMap* tmx, const std::vector<std::vector<TileType>>& tileData) {
    _tileData = &tileData;
    _mapHeight = (int)tileData.size();
    _mapWidth = (int)tileData[0].size();
    
    int texWidth = _mapWidth * _tileSize;
    int texHeight = _mapHeight * _tileSize;
    
    // 使用RenderTexture批量渲染减少draw calls
    // Layer 1: 背景+地形 合并到一个RenderTexture
    auto mainRT = RenderTexture::create(texWidth, texHeight);
    mainRT->beginWithClear(0.15f, 0.12f, 0.1f, 1.0f);  // 深棕色背景
    renderBackground();
    renderTerrain();
    mainRT->end();
    
    auto mainSprite = Sprite::createWithTexture(mainRT->getSprite()->getTexture());
    mainSprite->setAnchorPoint(Vec2::ZERO);
    mainSprite->setFlippedY(true);
    this->addChild(mainSprite, Z_TERRAIN);
    
    // Layer 2: 装饰 单独渲染
    renderDecorationsDirect();
    
    _tileData = nullptr;
}

// =====================================================
// LAYER 1: 背景 - Dead Cells风格的背景墙
// =====================================================
void TileRenderer::renderBackground() {
    // 大型背景装饰（壁龛、窗户、拱门等）
    std::vector<std::string> bgLargeDecos = {
        "wall_decorations/prisonBigAlcove", "wall_decorations/alchemyAlcove",
        "wall_decorations/archeSmall", "wall_decorations/prionSmallWindow"
    };
    
    // 基础背景墙
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if ((*_tileData)[y][x] != TileType::AIR) continue;
            
            // 使用backWall贴图（绿色砖块风格）+ 位置变化
            int variant = getBlockHash(x, y, BLOCK_BACKGROUND, 0) % _backWallTiles.size();
            auto sprite = createTileSprite(_backWallTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2::ZERO);
                sprite->setPosition(Vec2(x * _tileSize, y * _tileSize));
                sprite->visit();
            }
        }
    }
    
    // 大型背景装饰（非常稀疏，每个房间最多1-2个）
    int bgDecoCount = 0;
    int maxBgDecos = 2;
    for (int y = 5; y < _mapHeight - 5 && bgDecoCount < maxBgDecos; y += 8) {
        for (int x = 5; x < _mapWidth - 5 && bgDecoCount < maxBgDecos; x += 12) {
            if ((*_tileData)[y][x] != TileType::AIR) continue;
            
            // 确保周围有足够空间（5x5空气区域）
            bool hasSpace = true;
            for (int dy = -2; dy <= 2 && hasSpace; dy++) {
                for (int dx = -2; dx <= 2 && hasSpace; dx++) {
                    int ny = y + dy, nx = x + dx;
                    if (ny < 0 || ny >= _mapHeight || nx < 0 || nx >= _mapWidth) continue;
                    if ((*_tileData)[ny][nx] != TileType::AIR) hasSpace = false;
                }
            }
            if (!hasSpace) continue;
            
            int h = hash(x, y, 200);
            if (h % 100 >= 30) continue;  // 30%概率（但间隔大所以实际很少）
            
            int variant = h % bgLargeDecos.size();
            auto sprite = createNaturalSprite(bgLargeDecos[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
                sprite->setPosition(Vec2(x * _tileSize + _tileSize / 2, y * _tileSize + _tileSize / 2));
                sprite->setOpacity(150);
                sprite->visit();
                bgDecoCount++;
            }
        }
    }
}

// =====================================================
// LAYER 2: 地形 - Dead Cells风格：使用贴图渲染
// =====================================================
void TileRenderer::renderTerrain() {
    auto getType = [&](int tx, int ty) -> TileType {
        if (ty < 0 || ty >= _mapHeight || tx < 0 || tx >= _mapWidth) return TileType::AIR;
        return (*_tileData)[ty][tx];
    };
    
    // 第一遍：用groundCells tileset渲染地面纹理
    // groundCells布局：第0行透明，第1行有地面纹理(col 0-21)
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if ((*_tileData)[y][x] != TileType::GROUND) continue;
            
            Vec2 pos(x * _tileSize, y * _tileSize);
            int variant = hash(x, y, 0) % 20;  // 使用第1行的前20个tile
            
            // 使用groundCells tileset第1行作为地面纹理
            if (_groundCellsTexture) {
                auto sprite = createTileFromSheet(_groundCellsTexture, variant, 1);
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    sprite->visit();
                }
            }
        }
    }
    
    // 第二遍：渲染地板表面边缘（使用rockStamp装饰）
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if ((*_tileData)[y][x] != TileType::GROUND) continue;
            
            Vec2 pos(x * _tileSize, y * _tileSize);
            
            bool solidAbove = (getType(x, y + 1) == TileType::GROUND);
            bool solidBelow = (getType(x, y - 1) == TileType::GROUND);
            bool solidLeft = (getType(x - 1, y) == TileType::GROUND);
            bool solidRight = (getType(x + 1, y) == TileType::GROUND);
            
            int h = hash(x, y, 10);
            
            // 地板表面 - 添加rockStamp装饰（更稀疏）
            if (!solidAbove) {
                if (h % 6 == 0) {  // 约16%概率
                    int variant = h % 4;
                    std::string stampName = "tiles/rockStamp" + std::to_string(variant);
                    auto stamp = createNaturalSprite(stampName);
                    if (stamp) {
                        stamp->setAnchorPoint(Vec2(0.5f, 0.0f));
                        stamp->setPosition(Vec2(pos.x + _tileSize/2, pos.y + _tileSize));
                        stamp->visit();
                    }
                }
                
                // 地板左角落 - rockStampCorner
                if (!solidLeft) {
                    auto corner = createNaturalSprite("tiles/rockStampCorner");
                    if (corner) {
                        corner->setAnchorPoint(Vec2(1.0f, 0.0f));
                        corner->setPosition(Vec2(pos.x, pos.y + _tileSize));
                        corner->visit();
                    }
                }
                // 地板右角落
                if (!solidRight) {
                    auto corner = createNaturalSprite("tiles/rockStampCorner");
                    if (corner) {
                        corner->setFlippedX(true);
                        corner->setAnchorPoint(Vec2(0.0f, 0.0f));
                        corner->setPosition(Vec2(pos.x + _tileSize, pos.y + _tileSize));
                        corner->visit();
                    }
                }
            }
            
            // 天花板 - 悬挂岩石装饰
            if (!solidBelow) {
                if (h % 8 == 0) {  // 约12%概率
                    auto stamp = createNaturalSprite("tiles/rockStamp0");
                    if (stamp) {
                        stamp->setFlippedY(true);
                        stamp->setAnchorPoint(Vec2(0.5f, 1.0f));
                        stamp->setPosition(Vec2(pos.x + _tileSize/2, pos.y));
                        stamp->visit();
                    }
                }
            }
            
            // 左墙边缘 - 使用rockStampSide（更稀疏）
            if (!solidLeft && solidAbove && solidBelow) {
                if (h % 5 == 0) {  // 约20%概率
                    int variant = h % 4;
                    std::string sideName = "tiles/rockStampSide" + std::to_string(variant);
                    auto side = createNaturalSprite(sideName);
                    if (side) {
                        side->setAnchorPoint(Vec2(1.0f, 0.5f));
                        side->setPosition(Vec2(pos.x, pos.y + _tileSize/2));
                        side->visit();
                    }
                }
            }
            
            // 右墙边缘
            if (!solidRight && solidAbove && solidBelow) {
                if (h % 5 == 0) {
                    int variant = h % 4;
                    std::string sideName = "tiles/rockStampSide" + std::to_string(variant);
                    auto side = createNaturalSprite(sideName);
                    if (side) {
                        side->setFlippedX(true);
                        side->setAnchorPoint(Vec2(0.0f, 0.5f));
                        side->setPosition(Vec2(pos.x + _tileSize, pos.y + _tileSize/2));
                        side->visit();
                    }
                }
            }
        }
    }
    
    // 第三遍：平台和梯子
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            TileType type = (*_tileData)[y][x];
            Vec2 pos(x * _tileSize, y * _tileSize);
            
            if (type == TileType::PLATFORM) {
                bool hasLeft = isPlatform(x - 1, y);
                bool hasRight = isPlatform(x + 1, y);
                
                Sprite* sprite = nullptr;
                int variant = getBlockHash(x, y, BLOCK_FLOOR, 3) % _platformTiles.size();
                
                if (!hasLeft && !hasRight) {
                    sprite = createTileSprite("platforms/woodenPlatform_2");
                } else if (!hasLeft) {
                    sprite = createTileSprite("platforms/woodenPlatform_left");
                } else if (!hasRight) {
                    sprite = createTileSprite("platforms/woodenPlatform_right");
                } else {
                    sprite = createTileSprite(_platformTiles[variant]);
                }
                
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    sprite->visit();
                }
                
                if (!hasLeft) {
                    auto corner = createTileSprite("platforms/woodenCornerLeft");
                    if (corner) {
                        corner->setAnchorPoint(Vec2::ZERO);
                        corner->setPosition(Vec2(pos.x, pos.y - _tileSize));
                        corner->visit();
                    }
                }
                if (!hasRight) {
                    auto corner = createTileSprite("platforms/woodenCornerRight");
                    if (corner) {
                        corner->setAnchorPoint(Vec2::ZERO);
                        corner->setPosition(Vec2(pos.x, pos.y - _tileSize));
                        corner->visit();
                    }
                }
            }
            else if (type == TileType::LADDER) {
                Sprite* sprite = nullptr;
                if (isSolid(x, y - 1)) {
                    sprite = createTileSprite("platforms/ladderFloor");
                } else if (isPlatform(x, y - 1)) {
                    sprite = createTileSprite("platforms/ladderPlatform");
                } else {
                    sprite = createTileSprite("platforms/ladderWall");
                }
                
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    sprite->visit();
                }
            }
        }
    }
}

// =====================================================
// LAYER 3: 装饰 - 严格控制密度和层级
// =====================================================
void TileRenderer::renderDecorations() {
    // 地面装饰：每8-12格一个，贴地放置
    int lastFloorDecoX = -15;
    for (int y = 1; y < _mapHeight; y++) {
        lastFloorDecoX = -15;
        for (int x = 0; x < _mapWidth; x++) {
            if (!isAir(x, y)) continue;
            bool onFloor = isSolid(x, y - 1);
            if (!onFloor) continue;
            
            int h = hash(x, y, 100);
            int interval = 8 + (h % 5);  // 8-12格间隔
            if (x - lastFloorDecoX < interval) continue;
            lastFloorDecoX = x;
            
            int variant = h % _floorDecoTiles.size();
            auto sprite = createNaturalSprite(_floorDecoTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2(0.5f, 0.0f));
                sprite->setPosition(Vec2(x * _tileSize + _tileSize / 2, y * _tileSize));
                sprite->visit();
            }
        }
    }
    
    // 天花板装饰：每10-15格一个
    int lastRoofDecoX = -20;
    for (int y = 0; y < _mapHeight - 1; y++) {
        lastRoofDecoX = -20;
        for (int x = 0; x < _mapWidth; x++) {
            if (!isAir(x, y)) continue;
            if (!isSolid(x, y + 1)) continue;
            
            int h = hash(x, y, 110);
            int interval = 10 + (h % 6);  // 10-15格间隔
            if (x - lastRoofDecoX < interval) continue;
            lastRoofDecoX = x;
            
            int variant = h % _roofDecoTiles.size();
            auto sprite = createNaturalSprite(_roofDecoTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2(0.5f, 1.0f));
                sprite->setPosition(Vec2(x * _tileSize + _tileSize / 2, (y + 1) * _tileSize));
                sprite->visit();
            }
        }
    }
    
    // 墙壁装饰：每10-15格一个，贴墙放置，不超出墙壁边缘
    for (int y = 2; y < _mapHeight - 2; y++) {
        for (int x = 1; x < _mapWidth - 1; x++) {
            if (!isSolid(x, y)) continue;
            
            EdgeType edge = getEdgeType(x, y);
            if (edge != EdgeType::WALL_LEFT && edge != EdgeType::WALL_RIGHT) continue;
            
            int h = hash(x, y, 120);
            if (h % 12 != 0) continue;  // 约1/12概率
            
            // 检查装饰位置在空气中
            int decoX = (edge == EdgeType::WALL_LEFT) ? x - 1 : x + 1;
            if (!isAir(decoX, y)) continue;
            
            int variant = h % _wallDecoTiles.size();
            auto sprite = createNaturalSprite(_wallDecoTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
                float posX = (edge == EdgeType::WALL_LEFT) ?
                    x * _tileSize - _tileSize / 4 :
                    (x + 1) * _tileSize + _tileSize / 4;
                sprite->setPosition(Vec2(posX, y * _tileSize + _tileSize / 2));
                if (edge == EdgeType::WALL_RIGHT) sprite->setFlippedX(true);
                sprite->visit();
            }
        }
    }
    
    // 蜘蛛网角落装饰：检测内角并放置蜘蛛网
    std::vector<std::string> webTL = {"webs/webBigTL", "webs/webMediumTL", "webs/webSmallTL"};
    std::vector<std::string> webTR = {"webs/webBigTR", "webs/webMediumTR", "webs/webSmallTR"};
    std::vector<std::string> webBL = {"webs/webBigBL", "webs/webMediumBL", "webs/webSmallBL"};
    std::vector<std::string> webBR = {"webs/webBigBR", "webs/webMediumBR", "webs/webSmallBR"};
    
    for (int y = 1; y < _mapHeight - 1; y++) {
        for (int x = 1; x < _mapWidth - 1; x++) {
            if (!isAir(x, y)) continue;
            
            int h = hash(x, y, 150);
            if (h % 100 >= 30) continue;  // 30%概率
            
            // 检测内角类型
            bool solidUp = isSolid(x, y + 1);
            bool solidDown = isSolid(x, y - 1);
            bool solidLeft = isSolid(x - 1, y);
            bool solidRight = isSolid(x + 1, y);
            
            Sprite* webSprite = nullptr;
            Vec2 webPos;
            
            // 左上角：上方和左方都是实心
            if (solidUp && solidLeft && !solidDown && !solidRight) {
                int variant = h % webTL.size();
                webSprite = createNaturalSprite(webTL[variant]);
                webPos = Vec2(x * _tileSize, (y + 1) * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(0.0f, 1.0f));
            }
            // 右上角：上方和右方都是实心
            else if (solidUp && solidRight && !solidDown && !solidLeft) {
                int variant = h % webTR.size();
                webSprite = createNaturalSprite(webTR[variant]);
                webPos = Vec2((x + 1) * _tileSize, (y + 1) * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(1.0f, 1.0f));
            }
            // 左下角：下方和左方都是实心
            else if (solidDown && solidLeft && !solidUp && !solidRight) {
                int variant = h % webBL.size();
                webSprite = createNaturalSprite(webBL[variant]);
                webPos = Vec2(x * _tileSize, y * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(0.0f, 0.0f));
            }
            // 右下角：下方和右方都是实心
            else if (solidDown && solidRight && !solidUp && !solidLeft) {
                int variant = h % webBR.size();
                webSprite = createNaturalSprite(webBR[variant]);
                webPos = Vec2((x + 1) * _tileSize, y * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(1.0f, 0.0f));
            }
            
            if (webSprite) {
                webSprite->setPosition(webPos);
                webSprite->visit();
            }
        }
    }
}

// =====================================================
// LAYER 4: 前景 - 链条、栏杆
// =====================================================
void TileRenderer::renderForeground() {
    // 链条：每20-30格一个
    int lastChainX = -30;
    for (int x = 3; x < _mapWidth - 3; x++) {
        int h = hash(x, 0, 200);
        int interval = 20 + (h % 11);  // 20-30格间隔
        if (x - lastChainX < interval) continue;
        
        // 找天花板
        for (int y = _mapHeight - 1; y >= 3; y--) {
            if (isAir(x, y) && isSolid(x, y + 1)) {
                lastChainX = x;
                
                // 链条（2-4格长）
                int chainLen = 2 + (h % 3);
                for (int i = 0; i < chainLen && y - i >= 0; i++) {
                    auto chain = createTileSprite(_chainTiles[0]);
                    if (chain) {
                        chain->setAnchorPoint(Vec2(0.5f, 1.0f));
                        chain->setPosition(Vec2(x * _tileSize + _tileSize / 2, (y - i + 1) * _tileSize));
                        chain->visit();
                    }
                }
                
                // 链条底部
                auto bot = createNaturalSprite("chains/chainBot_0");
                if (bot) {
                    bot->setAnchorPoint(Vec2(0.5f, 1.0f));
                    bot->setPosition(Vec2(x * _tileSize + _tileSize / 2, (y - chainLen + 1) * _tileSize));
                    bot->visit();
                }
                break;
            }
        }
    }
    
    // 栏杆：平台上，20%概率
    for (int y = 1; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if (!isAir(x, y)) continue;
            if (!isPlatform(x, y - 1)) continue;
            
            int h = hash(x, y, 210);
            if (h % 100 >= 20) continue;  // 20%概率
            
            int variant = h % _railingTiles.size();
            auto sprite = createTileSprite(_railingTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2::ZERO);
                sprite->setPosition(Vec2(x * _tileSize, y * _tileSize));
                sprite->visit();
            }
        }
    }
}

// =====================================================
// Direct渲染方法 - 直接添加sprite到节点
// =====================================================
void TileRenderer::renderBackgroundDirect() {
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if ((*_tileData)[y][x] != TileType::AIR) continue;
            
            int variant = getBlockHash(x, y, BLOCK_BACKGROUND, 0) % _backWallTiles.size();
            auto sprite = createTileSprite(_backWallTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2::ZERO);
                sprite->setPosition(Vec2(x * _tileSize, y * _tileSize));
                this->addChild(sprite, Z_BACKGROUND);
            }
        }
    }
}

void TileRenderer::renderTerrainDirect() {
    auto getType = [&](int tx, int ty) -> TileType {
        if (ty < 0 || ty >= _mapHeight || tx < 0 || tx >= _mapWidth) return TileType::AIR;
        return (*_tileData)[ty][tx];
    };
    
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if ((*_tileData)[y][x] != TileType::GROUND) continue;
            
            Vec2 pos(x * _tileSize, y * _tileSize);
            
            // 用深棕色填充地面（不用dirt贴图因为它是纯黑色）
            auto fill = LayerColor::create(Color4B(50, 40, 35, 255), _tileSize, _tileSize);
            fill->setPosition(pos);
            this->addChild(fill, Z_TERRAIN);
            
            bool solidAbove = (getType(x, y + 1) == TileType::GROUND);
            bool solidBelow = (getType(x, y - 1) == TileType::GROUND);
            bool solidLeft = (getType(x - 1, y) == TileType::GROUND);
            bool solidRight = (getType(x + 1, y) == TileType::GROUND);
            
            // 边缘贴图（透明背景+高光线）覆盖在填充色上
            int variant = getBlockHash(x, y, BLOCK_FLOOR, 1);
            
            // 地板表面高光
            if (!solidAbove) {
                auto sprite = createNaturalSprite(_groundTiles[variant % _groundTiles.size()]);
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    this->addChild(sprite, Z_TERRAIN + 1);
                }
            }
            // 天花板
            if (!solidBelow) {
                auto sprite = createNaturalSprite(_ceilingTiles[variant % _ceilingTiles.size()]);
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    this->addChild(sprite, Z_TERRAIN + 1);
                }
            }
            // 左墙边缘
            if (!solidLeft) {
                auto sprite = createNaturalSprite(_stoneWallTiles[variant % _stoneWallTiles.size()]);
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    this->addChild(sprite, Z_TERRAIN + 1);
                }
            }
            // 右墙边缘
            if (!solidRight) {
                auto sprite = createNaturalSprite(_stoneWallTiles[variant % _stoneWallTiles.size()]);
                if (sprite) {
                    sprite->setFlippedX(true);
                    sprite->setAnchorPoint(Vec2(1.0f, 0.0f));
                    sprite->setPosition(Vec2(pos.x + _tileSize, pos.y));
                    this->addChild(sprite, Z_TERRAIN + 1);
                }
            }
        }
    }
    
    // 平台和梯子
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            TileType type = (*_tileData)[y][x];
            Vec2 pos(x * _tileSize, y * _tileSize);
            
            if (type == TileType::PLATFORM) {
                bool hasLeft = isPlatform(x - 1, y);
                bool hasRight = isPlatform(x + 1, y);
                
                Sprite* sprite = nullptr;
                int variant = getBlockHash(x, y, BLOCK_FLOOR, 3) % _platformTiles.size();
                
                if (!hasLeft && !hasRight) {
                    sprite = createTileSprite("platforms/woodenPlatform_2");
                } else if (!hasLeft) {
                    sprite = createTileSprite("platforms/woodenPlatform_left");
                } else if (!hasRight) {
                    sprite = createTileSprite("platforms/woodenPlatform_right");
                } else {
                    sprite = createTileSprite(_platformTiles[variant]);
                }
                
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    this->addChild(sprite, Z_TERRAIN + 2);
                }
            }
            else if (type == TileType::LADDER) {
                Sprite* sprite = nullptr;
                if (isSolid(x, y - 1)) {
                    sprite = createTileSprite("platforms/ladderFloor");
                } else if (isPlatform(x, y - 1)) {
                    sprite = createTileSprite("platforms/ladderPlatform");
                } else {
                    sprite = createTileSprite("platforms/ladderWall");
                }
                
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    this->addChild(sprite, Z_TERRAIN + 2);
                }
            }
        }
    }
}

void TileRenderer::renderDecorationsDirect() {
    // 大型地面装饰物（药瓶、木桶、骷髅等）
    std::vector<std::string> largeFloorDecos = {
        "alchemy/fioleAliquidGreen", "alchemy/fioleAliquidRed", "alchemy/fioleAliquidPurple",
        "alchemy/fioleBliquidGreen", "alchemy/fioleCliquidRed",
        "landmarks/dummy_0", "landmarks/skeletonSheet"
    };
    
    // 悬挂装饰（笼子、链条）
    std::vector<std::string> hangingDecos = {
        "chains/suspendedCage", "chains/chainBot_0", "chains/chainBot_1", 
        "chains/chainBot_2", "chains/chainBot_3"
    };
    
    // 墙壁装饰（书架、旗帜、窗户）
    std::vector<std::string> wallDecos = {
        "wall_decorations/shelf_0", "wall_decorations/shelf_1",
        "wall_decorations/bannerBig", "wall_decorations/prionSmallWindow",
        "wall_decorations/stampWallBig_0", "wall_decorations/stampWallBig_1"
    };
    
    // 地面小装饰
    int lastFloorDecoX = -15;
    for (int y = 1; y < _mapHeight; y++) {
        lastFloorDecoX = -15;
        for (int x = 0; x < _mapWidth; x++) {
            if (!isAir(x, y)) continue;
            if (!isSolid(x, y - 1)) continue;
            
            int h = hash(x, y, 100);
            int interval = 6 + (h % 4);
            if (x - lastFloorDecoX < interval) continue;
            lastFloorDecoX = x;
            
            int variant = h % _floorDecoTiles.size();
            auto sprite = createNaturalSprite(_floorDecoTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2(0.5f, 0.0f));
                sprite->setPosition(Vec2(x * _tileSize + _tileSize / 2, y * _tileSize));
                this->addChild(sprite, Z_DECORATION);
            }
        }
    }
    
    // 大型地面装饰物（每行最多1个）
    for (int y = 1; y < _mapHeight; y++) {
        bool placedThisRow = false;
        for (int x = 5; x < _mapWidth - 5 && !placedThisRow; x++) {
            if (!isAir(x, y)) continue;
            if (!isSolid(x, y - 1)) continue;
            if (!isAir(x, y + 1)) continue;
            
            int h = hash(x, y, 150);
            if (h % 100 >= 5) continue;  // 5%概率
            
            int variant = h % largeFloorDecos.size();
            auto sprite = createNaturalSprite(largeFloorDecos[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2(0.5f, 0.0f));
                sprite->setPosition(Vec2(x * _tileSize + _tileSize / 2, y * _tileSize));
                this->addChild(sprite, Z_DECORATION + 1);
                placedThisRow = true;
            }
        }
    }
    
    // 天花板装饰（苔藓等）
    for (int y = 0; y < _mapHeight - 1; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if (!isAir(x, y)) continue;
            if (!isSolid(x, y + 1)) continue;
            
            int h = hash(x, y, 110);
            if (h % 10 != 0) continue;  // 10%概率
            
            int variant = h % _roofDecoTiles.size();
            auto sprite = createNaturalSprite(_roofDecoTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2(0.5f, 1.0f));
                sprite->setPosition(Vec2(x * _tileSize + _tileSize / 2, (y + 1) * _tileSize));
                this->addChild(sprite, Z_DECORATION);
            }
        }
    }
    
    // 悬挂装饰（笼子、链条）- 每个房间最多2个，带摇摆动画
    int hangingCount = 0;
    for (int y = 0; y < _mapHeight - 3 && hangingCount < 2; y++) {
        for (int x = 5; x < _mapWidth - 5 && hangingCount < 2; x += 10) {
            if (!isAir(x, y)) continue;
            if (!isSolid(x, y + 1)) continue;
            if (!isAir(x, y - 1) || !isAir(x, y - 2)) continue;
            
            int h = hash(x, y, 160);
            if (h % 100 >= 20) continue;
            
            int variant = h % hangingDecos.size();
            auto sprite = createNaturalSprite(hangingDecos[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2(0.5f, 1.0f));
                sprite->setPosition(Vec2(x * _tileSize + _tileSize / 2, (y + 1) * _tileSize));
                this->addChild(sprite, Z_DECORATION + 2);
                
                // 添加轻微摇摆动画
                float swingAngle = 3.0f + (h % 3);  // 3-5度
                float swingDuration = 2.0f + (h % 10) / 10.0f;  // 2-3秒
                auto swingLeft = RotateTo::create(swingDuration, -swingAngle);
                auto swingRight = RotateTo::create(swingDuration, swingAngle);
                auto swingSeq = Sequence::create(swingLeft, swingRight, nullptr);
                auto swingForever = RepeatForever::create(swingSeq);
                sprite->runAction(swingForever);
                
                hangingCount++;
            }
        }
    }
    
    // 墙壁装饰（每面墙最多1个）
    int wallDecoCount = 0;
    for (int y = 3; y < _mapHeight - 3 && wallDecoCount < 3; y += 4) {
        for (int x = 1; x < _mapWidth - 1 && wallDecoCount < 3; x++) {
            if (!isSolid(x, y)) continue;
            if (!isSolid(x, y + 1) || !isSolid(x, y - 1)) continue;
            
            bool leftWall = !isSolid(x - 1, y) && isAir(x - 1, y);
            bool rightWall = !isSolid(x + 1, y) && isAir(x + 1, y);
            if (!leftWall && !rightWall) continue;
            
            int h = hash(x, y, 170);
            if (h % 100 >= 25) continue;  // 25%概率（但间隔大）
            
            int variant = h % wallDecos.size();
            auto sprite = createNaturalSprite(wallDecos[variant]);
            if (sprite) {
                if (leftWall) {
                    sprite->setAnchorPoint(Vec2(1.0f, 0.5f));
                    sprite->setPosition(Vec2(x * _tileSize, y * _tileSize + _tileSize / 2));
                } else {
                    sprite->setFlippedX(true);
                    sprite->setAnchorPoint(Vec2(0.0f, 0.5f));
                    sprite->setPosition(Vec2((x + 1) * _tileSize, y * _tileSize + _tileSize / 2));
                }
                this->addChild(sprite, Z_DECORATION);
                wallDecoCount++;
            }
        }
    }
    
    // 蜘蛛网角落（每个房间最多3个）
    std::vector<std::string> webTL = {"webs/webBigTL", "webs/webMediumTL", "webs/webSmallTL"};
    std::vector<std::string> webTR = {"webs/webBigTR", "webs/webMediumTR", "webs/webSmallTR"};
    
    int webCount = 0;
    for (int y = 1; y < _mapHeight - 1 && webCount < 3; y++) {
        for (int x = 1; x < _mapWidth - 1 && webCount < 3; x++) {
            if (!isAir(x, y)) continue;
            
            int h = hash(x, y, 180);
            if (h % 100 >= 8) continue;  // 8%概率
            
            bool solidUp = isSolid(x, y + 1);
            bool solidLeft = isSolid(x - 1, y);
            bool solidRight = isSolid(x + 1, y);
            
            Sprite* webSprite = nullptr;
            Vec2 webPos;
            
            // 左上角
            if (solidUp && solidLeft && !solidRight) {
                int v = h % webTL.size();
                webSprite = createNaturalSprite(webTL[v]);
                webPos = Vec2(x * _tileSize, (y + 1) * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(0.0f, 1.0f));
            }
            // 右上角
            else if (solidUp && solidRight && !solidLeft) {
                int v = h % webTR.size();
                webSprite = createNaturalSprite(webTR[v]);
                webPos = Vec2((x + 1) * _tileSize, (y + 1) * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(1.0f, 1.0f));
            }
            
            if (webSprite) {
                webSprite->setPosition(webPos);
                this->addChild(webSprite, Z_DECORATION);
                webCount++;
            }
        }
    }
}
