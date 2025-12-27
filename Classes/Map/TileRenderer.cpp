#include "TileRenderer.h"

// Z-order levels - negative to ensure below characters
const int Z_BACKGROUND = -300;
const int Z_TERRAIN = -200;
const int Z_DECORATION = -150;
const int Z_FOREGROUND = -100;

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
    
    // Load groundCells tileset
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
    // Ground tiles
    _groundTiles = {"tiles/ground_0", "tiles/ground_1", "tiles/ground_2", "tiles/ground_3"};
    _groundLeftTiles = {"tiles/ground_left_0", "tiles/ground_left_1"};
    _groundRightTiles = {"tiles/ground_right_0", "tiles/ground_right_1"};
    
    // Ceiling tiles
    _ceilingTiles = {"tiles/ceiling_0", "tiles/ceiling_1", "tiles/ceiling_2", "tiles/ceiling_3"};
    
    // Background wall tiles
    _backWallTiles = {
        "tiles/backWall_0", "tiles/backWall_1", "tiles/backWall_2",
        "tiles/backWall_3", "tiles/backWall_4", "tiles/backWall_5"
    };
    
    // Stone wall tiles
    _stoneWallTiles = {"tiles/stoneWall_0", "tiles/stoneWall_1", "tiles/stoneWall_2", "tiles/stoneWall_3"};
    
    // Platform tiles
    _platformTiles = {
        "platforms/woodenPlatform_0", "platforms/woodenPlatform_1",
        "platforms/woodenPlatform_2", "platforms/woodenPlatform_3", "platforms/woodenPlatform_4"
    };

    // Floor decoration tiles
    _floorDecoTiles = {
        "floor_decorations/floorjunk_0", "floor_decorations/floorjunk_1",
        "floor_decorations/floorjunk_2", "floor_decorations/floorjunk_3",
        "floor_decorations/floorjunk_4", "floor_decorations/floorjunk_5",
        "plants/leaf_0", "plants/leaf_1", "plants/leaf_2"
    };
    
    // Wall decoration tiles
    _wallDecoTiles = {
        "wall_decorations/wallJunk_0", "wall_decorations/wallJunk_1",
        "wall_decorations/wallJunk_2", "wall_decorations/wallJunk_3",
        "wall_decorations/stampWallSmall_0", "wall_decorations/stampWallSmall_1"
    };
    
    // Roof decoration tiles
    _roofDecoTiles = {
        "plants/roofMoss_0", "plants/roofMoss_1", "plants/roofMoss_2",
        "wall_decorations/stampCeilingSmall_0", "wall_decorations/stampCeilingSmall_1"
    };
    
    // Chain tiles
    _chainTiles = {"chains/chainTile"};
    
    // Railing tiles
    _railingTiles = {"npcs/railing_0", "npcs/railing_1", "npcs/railing_2", "npcs/railing_3"};
}

// Hash function
int TileRenderer::hash(int x, int y, int salt) {
    int h = x * 374761393 + y * 668265263 + salt;
    h = (h ^ (h >> 13)) * 1274126177;
    return abs(h ^ (h >> 16));
}

// Block hash: same block returns same value
int TileRenderer::getBlockHash(int x, int y, int blockSize, int salt) {
    int bx = x / blockSize;
    int by = y / blockSize;
    return hash(bx, by, salt);
}

// Sprite creation
Sprite* TileRenderer::createTileSprite(const std::string& name) {
    if (name.empty()) return nullptr;
    std::string path = _basePath + name + "-=-0-=-.png";
    auto sprite = Sprite::create(path);
    if (!sprite) {
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
    std::string path = _basePath + name + "-=-0-=-.png";
    auto sprite = Sprite::create(path);
    if (!sprite) {
        path = _basePath + name.substr(name.find_last_of('/') + 1) + "-=-0-=-.png";
        sprite = Sprite::create(path);
    }
    return sprite;
}

Sprite* TileRenderer::createTileFromSheet(Texture2D* sheet, int col, int row) {
    if (!sheet) return nullptr;
    Rect rect(col * _tileSize, row * _tileSize, _tileSize, _tileSize);
    auto sprite = Sprite::createWithTexture(sheet, rect);
    return sprite;
}

// Helper functions
bool TileRenderer::isSolid(int x, int y) {
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

int TileRenderer::countFloorLeft(int x, int y) {
    int count = 0;
    for (int i = x - 1; i >= 0; i--) {
        if (isSolid(i, y) && !isSolid(i, y + 1)) count++;
        else break;
    }
    return count;
}

int TileRenderer::countFloorRight(int x, int y) {
    int count = 0;
    for (int i = x + 1; i < _mapWidth; i++) {
        if (isSolid(i, y) && !isSolid(i, y + 1)) count++;
        else break;
    }
    return count;
}

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

EdgeType TileRenderer::getEdgeType(int x, int y) {
    if (!_tileData) return EdgeType::NONE;
    if (y < 0 || y >= _mapHeight || x < 0 || x >= _mapWidth) return EdgeType::NONE;
    if ((*_tileData)[y][x] != TileType::GROUND) return EdgeType::NONE;
    
    auto getType = [&](int tx, int ty) -> TileType {
        if (ty < 0 || ty >= _mapHeight || tx < 0 || tx >= _mapWidth) return TileType::AIR;
        return (*_tileData)[ty][tx];
    };
    
    bool solidAbove = (getType(x, y + 1) == TileType::GROUND);
    bool solidBelow = (getType(x, y - 1) == TileType::GROUND);
    bool solidLeft = (getType(x - 1, y) == TileType::GROUND);
    bool solidRight = (getType(x + 1, y) == TileType::GROUND);
    
    if (solidAbove && solidBelow && solidLeft && solidRight) {
        return EdgeType::NONE;
    }
    
    if (!solidAbove) {
        if (!solidLeft && !solidRight) return EdgeType::FLOOR_SINGLE;
        if (!solidLeft) return EdgeType::FLOOR_LEFT;
        if (!solidRight) return EdgeType::FLOOR_RIGHT;
        return EdgeType::FLOOR;
    }
    
    if (!solidBelow) {
        return EdgeType::CEILING;
    }
    
    if (!solidLeft) return EdgeType::WALL_LEFT;
    if (!solidRight) return EdgeType::WALL_RIGHT;
    
    return EdgeType::FLOOR;
}


// Main render entry
void TileRenderer::renderFromCollisionLayer(TMXTiledMap* tmx, const std::vector<std::vector<TileType>>& tileData) {
    _tileData = &tileData;
    _mapHeight = (int)tileData.size();
    _mapWidth = (int)tileData[0].size();
    
    int texWidth = _mapWidth * _tileSize;
    int texHeight = _mapHeight * _tileSize;
    
    auto mainRT = RenderTexture::create(texWidth, texHeight);
    mainRT->beginWithClear(0.15f, 0.12f, 0.1f, 1.0f);
    renderBackground();
    renderTerrain();
    mainRT->end();
    
    auto mainSprite = Sprite::createWithTexture(mainRT->getSprite()->getTexture());
    mainSprite->setAnchorPoint(Vec2::ZERO);
    mainSprite->setFlippedY(true);
    this->addChild(mainSprite, Z_TERRAIN);
    
    renderDecorationsDirect();
    
    _tileData = nullptr;
}

// Background rendering
void TileRenderer::renderBackground() {
    std::vector<std::string> bgLargeDecos = {
        "wall_decorations/prisonBigAlcove", "wall_decorations/alchemyAlcove",
        "wall_decorations/archeSmall", "wall_decorations/prionSmallWindow"
    };
    
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if ((*_tileData)[y][x] != TileType::AIR) continue;
            
            int variant = getBlockHash(x, y, BLOCK_BACKGROUND, 0) % _backWallTiles.size();
            auto sprite = createTileSprite(_backWallTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2::ZERO);
                sprite->setPosition(Vec2(x * _tileSize, y * _tileSize));
                sprite->visit();
            }
        }
    }
    
    int bgDecoCount = 0;
    int maxBgDecos = 2;
    for (int y = 5; y < _mapHeight - 5 && bgDecoCount < maxBgDecos; y += 8) {
        for (int x = 5; x < _mapWidth - 5 && bgDecoCount < maxBgDecos; x += 12) {
            if ((*_tileData)[y][x] != TileType::AIR) continue;
            
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
            if (h % 100 >= 30) continue;
            
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

// Terrain rendering
void TileRenderer::renderTerrain() {
    auto getType = [&](int tx, int ty) -> TileType {
        if (ty < 0 || ty >= _mapHeight || tx < 0 || tx >= _mapWidth) return TileType::AIR;
        return (*_tileData)[ty][tx];
    };
    
    // First pass: ground texture
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if ((*_tileData)[y][x] != TileType::GROUND) continue;
            
            Vec2 pos(x * _tileSize, y * _tileSize);
            int variant = hash(x, y, 0) % 20;
            
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
    
    // Second pass: edge decorations
    for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if ((*_tileData)[y][x] != TileType::GROUND) continue;
            
            Vec2 pos(x * _tileSize, y * _tileSize);
            
            bool solidAbove = (getType(x, y + 1) == TileType::GROUND);
            bool solidBelow = (getType(x, y - 1) == TileType::GROUND);
            bool solidLeft = (getType(x - 1, y) == TileType::GROUND);
            bool solidRight = (getType(x + 1, y) == TileType::GROUND);
            
            int h = hash(x, y, 10);
            
            // Floor surface
            if (!solidAbove) {
                if (h % 6 == 0) {
                    int variant = h % 4;
                    std::string stampName = "tiles/rockStamp" + std::to_string(variant);
                    auto stamp = createNaturalSprite(stampName);
                    if (stamp) {
                        stamp->setAnchorPoint(Vec2(0.5f, 0.0f));
                        stamp->setPosition(Vec2(pos.x + _tileSize/2, pos.y + _tileSize));
                        stamp->visit();
                    }
                }
                
                // Floor left corner
                if (!solidLeft) {
                    auto corner = createNaturalSprite("tiles/rockStampCorner");
                    if (corner) {
                        corner->setAnchorPoint(Vec2(1.0f, 0.0f));
                        corner->setPosition(Vec2(pos.x, pos.y + _tileSize));
                        corner->visit();
                    }
                }
                // Floor right corner
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
            
            // Ceiling
            if (!solidBelow) {
                if (h % 8 == 0) {
                    auto stamp = createNaturalSprite("tiles/rockStamp0");
                    if (stamp) {
                        stamp->setFlippedY(true);
                        stamp->setAnchorPoint(Vec2(0.5f, 1.0f));
                        stamp->setPosition(Vec2(pos.x + _tileSize/2, pos.y));
                        stamp->visit();
                    }
                }
            }
            
            // Left wall edge
            if (!solidLeft && solidAbove && solidBelow) {
                if (h % 5 == 0) {
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
            
            // Right wall edge
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
    
    // Third pass: platforms and ladders
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


// Decorations rendering
void TileRenderer::renderDecorations() {
    // Floor decorations
    int lastFloorDecoX = -15;
    for (int y = 1; y < _mapHeight; y++) {
        lastFloorDecoX = -15;
        for (int x = 0; x < _mapWidth; x++) {
            if (!isAir(x, y)) continue;
            bool onFloor = isSolid(x, y - 1);
            if (!onFloor) continue;
            
            int h = hash(x, y, 100);
            int interval = 8 + (h % 5);
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
    
    // Roof decorations
    int lastRoofDecoX = -20;
    for (int y = 0; y < _mapHeight - 1; y++) {
        lastRoofDecoX = -20;
        for (int x = 0; x < _mapWidth; x++) {
            if (!isAir(x, y)) continue;
            if (!isSolid(x, y + 1)) continue;
            
            int h = hash(x, y, 110);
            int interval = 10 + (h % 6);
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
    
    // Wall decorations
    for (int y = 2; y < _mapHeight - 2; y++) {
        for (int x = 1; x < _mapWidth - 1; x++) {
            if (!isSolid(x, y)) continue;
            
            EdgeType edge = getEdgeType(x, y);
            if (edge != EdgeType::WALL_LEFT && edge != EdgeType::WALL_RIGHT) continue;
            
            int h = hash(x, y, 120);
            if (h % 12 != 0) continue;
            
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
    
    // Spider webs
    std::vector<std::string> webTL = {"webs/webBigTL", "webs/webMediumTL", "webs/webSmallTL"};
    std::vector<std::string> webTR = {"webs/webBigTR", "webs/webMediumTR", "webs/webSmallTR"};
    std::vector<std::string> webBL = {"webs/webBigBL", "webs/webMediumBL", "webs/webSmallBL"};
    std::vector<std::string> webBR = {"webs/webBigBR", "webs/webMediumBR", "webs/webSmallBR"};
    
    for (int y = 1; y < _mapHeight - 1; y++) {
        for (int x = 1; x < _mapWidth - 1; x++) {
            if (!isAir(x, y)) continue;
            
            int h = hash(x, y, 150);
            if (h % 100 >= 30) continue;
            
            bool solidUp = isSolid(x, y + 1);
            bool solidDown = isSolid(x, y - 1);
            bool solidLeft = isSolid(x - 1, y);
            bool solidRight = isSolid(x + 1, y);
            
            Sprite* webSprite = nullptr;
            Vec2 webPos;
            
            // Top-left corner
            if (solidUp && solidLeft && !solidDown && !solidRight) {
                int variant = h % webTL.size();
                webSprite = createNaturalSprite(webTL[variant]);
                webPos = Vec2(x * _tileSize, (y + 1) * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(0.0f, 1.0f));
            }
            // Top-right corner
            else if (solidUp && solidRight && !solidDown && !solidLeft) {
                int variant = h % webTR.size();
                webSprite = createNaturalSprite(webTR[variant]);
                webPos = Vec2((x + 1) * _tileSize, (y + 1) * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(1.0f, 1.0f));
            }
            // Bottom-left corner
            else if (solidDown && solidLeft && !solidUp && !solidRight) {
                int variant = h % webBL.size();
                webSprite = createNaturalSprite(webBL[variant]);
                webPos = Vec2(x * _tileSize, y * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(0.0f, 0.0f));
            }
            // Bottom-right corner
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

// Foreground rendering
void TileRenderer::renderForeground() {
    // Chains
    int lastChainX = -30;
    for (int x = 3; x < _mapWidth - 3; x++) {
        int h = hash(x, 0, 200);
        int interval = 20 + (h % 11);
        if (x - lastChainX < interval) continue;
        
        for (int y = _mapHeight - 1; y >= 3; y--) {
            if (isAir(x, y) && isSolid(x, y + 1)) {
                lastChainX = x;
                
                int chainLen = 2 + (h % 3);
                for (int i = 0; i < chainLen && y - i >= 0; i++) {
                    auto chain = createTileSprite(_chainTiles[0]);
                    if (chain) {
                        chain->setAnchorPoint(Vec2(0.5f, 1.0f));
                        chain->setPosition(Vec2(x * _tileSize + _tileSize / 2, (y - i + 1) * _tileSize));
                        chain->visit();
                    }
                }
                
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
    
    // Railings
    for (int y = 1; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if (!isAir(x, y)) continue;
            if (!isPlatform(x, y - 1)) continue;
            
            int h = hash(x, y, 210);
            if (h % 100 >= 20) continue;
            
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


// Direct rendering methods
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
            
            auto fill = LayerColor::create(Color4B(50, 40, 35, 255), _tileSize, _tileSize);
            fill->setPosition(pos);
            this->addChild(fill, Z_TERRAIN);
            
            bool solidAbove = (getType(x, y + 1) == TileType::GROUND);
            bool solidBelow = (getType(x, y - 1) == TileType::GROUND);
            bool solidLeft = (getType(x - 1, y) == TileType::GROUND);
            bool solidRight = (getType(x + 1, y) == TileType::GROUND);
            
            int variant = getBlockHash(x, y, BLOCK_FLOOR, 1);
            
            // Floor surface
            if (!solidAbove) {
                auto sprite = createNaturalSprite(_groundTiles[variant % _groundTiles.size()]);
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    this->addChild(sprite, Z_TERRAIN + 1);
                }
            }
            // Ceiling
            if (!solidBelow) {
                auto sprite = createNaturalSprite(_ceilingTiles[variant % _ceilingTiles.size()]);
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    this->addChild(sprite, Z_TERRAIN + 1);
                }
            }
            // Left wall edge
            if (!solidLeft) {
                auto sprite = createNaturalSprite(_stoneWallTiles[variant % _stoneWallTiles.size()]);
                if (sprite) {
                    sprite->setAnchorPoint(Vec2::ZERO);
                    sprite->setPosition(pos);
                    this->addChild(sprite, Z_TERRAIN + 1);
                }
            }
            // Right wall edge
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
    
    // Platforms and ladders
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
    std::vector<std::string> largeFloorDecos = {
        "alchemy/fioleAliquidGreen", "alchemy/fioleAliquidRed", "alchemy/fioleAliquidPurple",
        "alchemy/fioleBliquidGreen", "alchemy/fioleCliquidRed",
        "landmarks/dummy_0", "landmarks/skeletonSheet"
    };
    
    std::vector<std::string> hangingDecos = {
        "chains/suspendedCage", "chains/chainBot_0", "chains/chainBot_1", 
        "chains/chainBot_2", "chains/chainBot_3"
    };
    
    std::vector<std::string> wallDecos = {
        "wall_decorations/shelf_0", "wall_decorations/shelf_1",
        "wall_decorations/bannerBig", "wall_decorations/prionSmallWindow",
        "wall_decorations/stampWallBig_0", "wall_decorations/stampWallBig_1"
    };
    
    // Floor decorations
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
    
    // Large floor decorations
    for (int y = 1; y < _mapHeight; y++) {
        bool placedThisRow = false;
        for (int x = 5; x < _mapWidth - 5 && !placedThisRow; x++) {
            if (!isAir(x, y)) continue;
            if (!isSolid(x, y - 1)) continue;
            if (!isAir(x, y + 1)) continue;
            
            int h = hash(x, y, 150);
            if (h % 100 >= 5) continue;
            
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
    
    // Ceiling decorations
    for (int y = 0; y < _mapHeight - 1; y++) {
        for (int x = 0; x < _mapWidth; x++) {
            if (!isAir(x, y)) continue;
            if (!isSolid(x, y + 1)) continue;
            
            int h = hash(x, y, 110);
            if (h % 10 != 0) continue;
            
            int variant = h % _roofDecoTiles.size();
            auto sprite = createNaturalSprite(_roofDecoTiles[variant]);
            if (sprite) {
                sprite->setAnchorPoint(Vec2(0.5f, 1.0f));
                sprite->setPosition(Vec2(x * _tileSize + _tileSize / 2, (y + 1) * _tileSize));
                this->addChild(sprite, Z_DECORATION);
            }
        }
    }
    
    // Hanging decorations
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
                
                float swingAngle = 3.0f + (h % 3);
                float swingDuration = 2.0f + (h % 10) / 10.0f;
                auto swingLeft = RotateTo::create(swingDuration, -swingAngle);
                auto swingRight = RotateTo::create(swingDuration, swingAngle);
                auto swingSeq = Sequence::create(swingLeft, swingRight, nullptr);
                auto swingForever = RepeatForever::create(swingSeq);
                sprite->runAction(swingForever);
                
                hangingCount++;
            }
        }
    }
    
    // Wall decorations
    int wallDecoCount = 0;
    for (int y = 3; y < _mapHeight - 3 && wallDecoCount < 3; y += 4) {
        for (int x = 1; x < _mapWidth - 1 && wallDecoCount < 3; x++) {
            if (!isSolid(x, y)) continue;
            if (!isSolid(x, y + 1) || !isSolid(x, y - 1)) continue;
            
            bool leftWall = !isSolid(x - 1, y) && isAir(x - 1, y);
            bool rightWall = !isSolid(x + 1, y) && isAir(x + 1, y);
            if (!leftWall && !rightWall) continue;
            
            int h = hash(x, y, 170);
            if (h % 100 >= 25) continue;
            
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
    
    // Spider webs
    std::vector<std::string> webTL = {"webs/webBigTL", "webs/webMediumTL", "webs/webSmallTL"};
    std::vector<std::string> webTR = {"webs/webBigTR", "webs/webMediumTR", "webs/webSmallTR"};
    
    int webCount = 0;
    for (int y = 1; y < _mapHeight - 1 && webCount < 3; y++) {
        for (int x = 1; x < _mapWidth - 1 && webCount < 3; x++) {
            if (!isAir(x, y)) continue;
            
            int h = hash(x, y, 180);
            if (h % 100 >= 8) continue;
            
            bool solidUp = isSolid(x, y + 1);
            bool solidLeft = isSolid(x - 1, y);
            bool solidRight = isSolid(x + 1, y);
            
            Sprite* webSprite = nullptr;
            Vec2 webPos;
            
            // Top-left corner
            if (solidUp && solidLeft && !solidRight) {
                int v = h % webTL.size();
                webSprite = createNaturalSprite(webTL[v]);
                webPos = Vec2(x * _tileSize, (y + 1) * _tileSize);
                if (webSprite) webSprite->setAnchorPoint(Vec2(0.0f, 1.0f));
            }
            // Top-right corner
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
