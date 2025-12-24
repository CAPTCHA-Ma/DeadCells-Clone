#include "RoomNode.h"

RoomNode* RoomNode::create(MapUnitData* data)
{

	auto node = new RoomNode();
	if (node && node->init(data)) return node;

	CC_SAFE_DELETE(node);

	return nullptr;

}

struct Vec2Hash
{

	std::size_t operator()(const cocos2d::Vec2& v) const
	{

		return std::hash<int>()((int)v.x) ^ (std::hash<int>()((int)v.y) << 1);

	}

};

struct Vec2Equal
{

	bool operator()(const cocos2d::Vec2& a, const cocos2d::Vec2& b) const
	{

		return (int)a.x == (int)b.x && (int)a.y == (int)b.y;

	}

} eq;

void GenBody(const std::vector<std::vector<PhysicsCategory>>& layerCategory, Node* layer)
{

	Size sz = Size(layerCategory[0].size(), layerCategory.size());
	std::vector<std::vector<bool>> visited(int(sz.height), std::vector<bool>(int(sz.width), false));

	for (int y = 0; y < sz.height; ++y)
	{

		for (int x = 0; x < sz.width; ++x)
		{

			if (visited[y][x] || layerCategory[y][x] == AIR) continue;

			int width = 0, height = 1, cateBitMask = 0, colBitMask = 0, conBitMask = 0;

			switch (layerCategory[y][x])
			{

				case GROUND:

					while (x + width < sz.width && layerCategory[y][x + width] == GROUND && !visited[y][x + width]) width++;
					while (y + height < sz.height)
					{

						bool rowIsFullWall = true;
						for (int k = 0; k < width; ++k) {

							if (layerCategory[y + height][x + k] != GROUND || visited[y + height][x + k])
							{

								rowIsFullWall = false;
								break;

							}

						}

						if (!rowIsFullWall)  break;

						height++;

					}

					cateBitMask = GROUND;
					colBitMask = PLAYER_BODY | ENEMY_BODY | ENEMY_BOMB | PLAYER_ARROW | ENEMY_ARROW
					conBitMask = ENEMY_BOMB | PLAYER_ARROW | ENEMY_ARROW;

					break;

				case PLATFORM:

					while (x + width < sz.width && layerCategory[y][x + width] == PLATFORM && !visited[y][x + width]) width++;

					cateBitMask = PLATFORM;
					colBitMask = PLAYER_BODY | ENEMY_BODY;
					conBitMask = PLAYER_BODY;

					break;

				case LADDER:

					while (y + height < sz.height && layerCategory[y + height][x] == LADDER && !visited[y + height][x]) height++;

					cateBitMask = LADDER;
					colBitMask = 0;
					conBitMask = PLAYER_BODY;

					break;

				default:
					break;

			}

			for (int y0 = y; y0 < y + height; ++y0)
			{

				for (int x0 = x; x0 < x + width; ++x0) visited[y0][x0] = true;

			}

			auto physicsBody = PhysicsBody::createBox(Size(width * 24, height * 24), PhysicsMaterial(0.1f, 0.0f, 0.0f));
			physicsBody->setDynamic(false);
			physicsBody->setCategoryBitmask(cateBitMask);
			physicsBody->setCollisionBitmask(colBitMask);
			physicsBody->setContactTestBitmask(conBitMask);

			auto node = Node::create();
			node->setPhysicsBody(physicsBody);

			Vec2 pos = Vec2(x, y) * 24;
			pos += Vec2(width * 24 / 2, height * 24 / 2);

			node->setPosition(pos);

			layer->addChild(node);

		}

	}

}

Node* GenCorridor(std::vector<std::vector<Vec2>>& paths, Vec2& origin)
{

	Node* corridorContainer = Node::create();

	std::unordered_map<Vec2, int, Vec2Hash, Vec2Equal> globalTiles;

	for (const auto& path : paths) 
	{

		for (const Vec2& pos : path) 
		{

			for (int dy = -2; dy <= 2; ++dy) 
			{

				for (int dx = -2; dx <= 2; ++dx) globalTiles[Vec2(pos.x + dx, pos.y + dy)] = 0; 

			}
		}

	}

	for (const auto& path : paths) 
	{

		int n = path.size();
		if (n < 2) continue;

		for (int i = 0; i < n; ++i) 
		{

			if (i == 0 || i == n - 1) continue;
			Vec2 pos = path[i];
			Vec2 dirToStart = path[i] - path[i - 1];
			Vec2 dirToEnd = (i + 1 < n) ? (path[i + 1] - path[i]) : Vec2::ZERO;

			for (int dy = -3; dy <= 3; ++dy) 
			{

				for (int dx = -3; dx <= 3; ++dx) 
				{

					if (abs(dx) == 3 || abs(dy) == 3) 
					{
						
						if (i <= 3) 
						{

							if (dirToStart.x > 0 && dx < 0) continue;
							if (dirToStart.x < 0 && dx > 0) continue;
							if (dirToStart.y > 0 && dy < 0) continue;
							if (dirToStart.y < 0 && dy > 0) continue;
						
						}

						if (i >= n - 4) {
							if (dirToEnd.x > 0 && dx > 0) continue;
							if (dirToEnd.x < 0 && dx < 0) continue;
							if (dirToEnd.y > 0 && dy > 0) continue;
							if (dirToEnd.y < 0 && dy < 0) continue;
						}

						Vec2 targetPos(pos.x + dx, pos.y + dy);
						
						if (globalTiles.find(targetPos) == globalTiles.end()) globalTiles[targetPos] = 1; 
						
					}

				}

			}

		}

	}

	std::unordered_set<Vec2, Vec2Hash, Vec2Equal> groundTiles;
	
	for (auto it = globalTiles.begin(); it != globalTiles.end(); ++it) 
	{

		if (it->second == 1) groundTiles.insert(it->first);
	}
	
	globalTiles.clear();

	while (!groundTiles.empty()) 
	{

		Vec2 start = *groundTiles.begin();
		int x = (int)start.x;
		int y = (int)start.y;


		int width = 1;
		while (groundTiles.find(Vec2(x + width, y)) != groundTiles.end()) width++;

		int height = 1;
		bool canExpandUp = true;
		while (canExpandUp)
		{

			for (int k = 0; k < width; ++k)
			{

				if (groundTiles.find(Vec2(x + k, y + height)) == groundTiles.end()) 
				{

					canExpandUp = false;
					break;

				}

			}

			if (canExpandUp) height++;

		}

		auto physicsBody = PhysicsBody::createBox(Size(width * 24, height * 24), PhysicsMaterial(0.1f, 0.0f, 0.0f));
		physicsBody->setDynamic(false);
		physicsBody->setCategoryBitmask(GROUND);
		physicsBody->setCollisionBitmask(PLAYER_BODY | ENEMY_BODY);

		auto node = Node::create();
		node->setPhysicsBody(physicsBody);
		node->setPosition((Vec2(x + width / 2.0f, y + height / 2.0f) - origin) * 24);
		corridorContainer->addChild(node);

		for (int row = 0; row < height; ++row) {
			for (int col = 0; col < width; ++col) {
				groundTiles.erase(Vec2(x + col, y + row));
			}
		}
	}

	return corridorContainer;
}

bool RoomNode::init(MapUnitData* data)
{

	auto tmx = TMXTiledMap::create(data->name);
	this->addChild(tmx);
	this->setAnchorPoint(Vec2::ZERO);
	this->setPosition(data->obstacle.lowLeft * 24);

	tmx->setAnchorPoint(Vec2::ZERO);

	Size Size = tmx->getMapSize();
	std::vector<std::vector<PhysicsCategory>> layerCategory(int(Size.height), std::vector<PhysicsCategory>(int(Size.width)));
	std::vector<std::vector<bool>> visited(int(Size.height), std::vector<bool>(int(Size.width), false));

	auto lnkLayer = tmx->getLayer("lnk");

	for (int x = 0; x < Size.width; x++)
	{

		for (int y = 0; y < Size.height; y++)
		{

			Vec2 tileCoord = Vec2(x, y);
			int gid = lnkLayer->getTileGIDAt(tileCoord);

			if (gid)
			{

				Value properties = tmx->getPropertiesForGID(gid);
				if (!properties.isNull())
				{

					ValueMap propsMap = properties.asValueMap();
					std::string tileType = propsMap.at("cate").asString();

					if (tileType == "DOOR") layerCategory[Size.height - y - 1][x] = PhysicsCategory::GROUND;

				}

			}

		}

	}

	std::queue<Vec2> q;
	if (data->chosenEntrance != -1) q.push(data->entrance[data->chosenEntrance]);
	if (data->chosenExit != -1) q.push(data->exit[data->chosenExit]);
	Vec2 dir[4] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };

	while (!q.empty())
	{

		for (int i = 0; i < 4; ++i)
		{

			Vec2 pos = q.front() + dir[i];
			if (pos.x < 0 || pos.x >= Size.width || pos.y < 0 || pos.y >= Size.height) continue;
			if (!visited[Size.height - pos.y - 1][pos.x] && layerCategory[Size.height - pos.y - 1][pos.x] == PhysicsCategory::GROUND)
			{

				visited[Size.height - pos.y - 1][pos.x] = true;
				layerCategory[Size.height - pos.y - 1][pos.x] = PhysicsCategory::AIR;
				q.push(pos);

			}

		}

		q.pop();

	}

	auto colLayer = tmx->getLayer("col");

	for (int x = 0; x < Size.width; x++)
	{

		for (int y = 0; y < Size.height; y++)
		{

			Vec2 tileCoord = Vec2(x, y);
			int gid = colLayer->getTileGIDAt(tileCoord);

			if (gid)
			{

				Value properties = tmx->getPropertiesForGID(gid);
				if (!properties.isNull())
				{

					ValueMap propsMap = properties.asValueMap();
					std::string tileType = propsMap.at("cate").asString();

					if (tileType == "GROUND")
					{

						layerCategory[Size.height - y - 1][x] = PhysicsCategory::GROUND;

					}
					else if (tileType == "LADDER")
					{
						layerCategory[Size.height - y - 1][x] = PhysicsCategory::LADDER;
					}
					else if (tileType == "PLATFORM")
					{
						layerCategory[Size.height - y - 1][x] = PhysicsCategory::PLATFORM;
					}
					else layerCategory[Size.height - y - 1][x] = PhysicsCategory::AIR;

				}
				else layerCategory[Size.height - y - 1][x] = PhysicsCategory::AIR;

			}
			else layerCategory[Size.height - y - 1][x] = PhysicsCategory::AIR;

		}

	}

	GenBody(layerCategory, tmx);

	if (data->nextRoom.size())
	{

		CCLOG("GENCOR!\n");

		std::vector<std::vector<Vec2>> paths;

		for (MapUnitData* nextRoom : data->nextRoom)
		{

			if (nextRoom->path.size()) paths.push_back(nextRoom->path);

		}

		if (paths.size())
		{

			CCLOG("ADD!\n");

			Node* layer = GenCorridor(paths, data->obstacle.lowLeft);
			this->addChild(layer);

		}

	}

	return true;

}