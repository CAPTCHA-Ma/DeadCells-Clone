#include "RoomNode.h"
#include "TileRenderer.h"

RoomNode* RoomNode::create(MapUnitData* data, cocos2d::Vector<MonsterLayer*>& _monsters)
{

	auto node = new RoomNode();
	if (node && node->init(data, _monsters))
	{
		node->autorelease();
		return node;
	}

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
					colBitMask = PLAYER_BODY | ENEMY_BODY | ENEMY_BOMB | PLAYER_ARROW | ENEMY_ARROW | WEAPON;
					conBitMask = ENEMY_BOMB | PLAYER_ARROW | ENEMY_ARROW | PLAYER_BODY |WEAPON;

					break;

				case PLATFORM:

					while (x + width < sz.width && layerCategory[y][x + width] == PLATFORM && !visited[y][x + width]) width++;

					cateBitMask = PLATFORM;
					colBitMask = PLAYER_BODY | ENEMY_BODY | ENEMY_BOMB | PLAYER_ARROW | ENEMY_ARROW;
					conBitMask = PLAYER_BODY;

					break;

				case LADDER:

					width = 1;

					while (y + height < sz.height && layerCategory[y + height][x] == LADDER && !visited[y + height][x]) height++;

					cateBitMask = LADDER;
					colBitMask = PLAYER_BODY;
					conBitMask = PLAYER_BODY;

					break;

				case MIX:

					width = 1;

					cateBitMask = MIX;
					colBitMask = PLAYER_BODY | ENEMY_BODY | ENEMY_BOMB | PLAYER_ARROW | ENEMY_ARROW;
					conBitMask = PLAYER_BODY;

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
			if (layerCategory[y][x] == LADDER)
			{

				for (auto& shape : physicsBody->getShapes())  shape->setSensor(true);

			}

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
	std::unordered_set<Vec2, Vec2Hash, Vec2Equal> airTiles;

	for (const auto& path : paths) 
	{

		for (const Vec2& pos : path) 
		{

			for (int dy = -2; dy <= 2; ++dy) 
			{

				for (int dx = -2; dx <= 2; ++dx) {
					Vec2 tilePos(pos.x + dx, pos.y + dy);
					globalTiles[tilePos] = 0;
					airTiles.insert(tilePos);
				}

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

		if (it->second == 1) {
			groundTiles.insert(it->first);
			airTiles.erase(it->first);
		}
	}
	
	globalTiles.clear();
	
	// Add background wall textures to corridor air areas
	for (const Vec2& airPos : airTiles) {
		int variant = RandomHelper::random_int(0, 5);
		std::string bgPath = "prison/tiles/backWall_" + std::to_string(variant) + "-=-0-=-.png";
		auto bgSprite = Sprite::create(bgPath);
		if (bgSprite) {
			bgSprite->setContentSize(Size(24, 24));
			Vec2 spritePos = (airPos - origin) * 24 + Vec2(12, 12);
			bgSprite->setPosition(spritePos);
			corridorContainer->addChild(bgSprite, -10);
		}
	}

	const int PLATFORM_INTERVAL = 4; 
	const int PLATFORM_WIDTH = 48; 
	const int PLATFORM_HEIGHT = 24; 

	for (const auto& path : paths)
	{

		int verticalDist = 0;

		if (path.size() < 4) continue;

		for (size_t i = 1; i < path.size() - 1; ++i)
		{
			Vec2 currentPos = path[i];
			Vec2 prevPos = path[i - 1];

			if (currentPos.x != prevPos.x)
			{
				verticalDist = 0;
				continue;
			}

			if (std::abs(currentPos.y - prevPos.y) > 0)
			{
				verticalDist += std::abs(currentPos.y - prevPos.y);

				if (verticalDist >= PLATFORM_INTERVAL)
				{

					verticalDist = 0;

					int side = (RandomHelper::random_int(0, 1) == 0) ? -1 : 1;

					bool wallExists = false;
					Vec2 wallPos = currentPos + Vec2(side * 3, 0);

					if (groundTiles.find(wallPos) != groundTiles.end()) {
						wallExists = true;
					}
					else {
						
						side = -side;
						wallPos = currentPos + Vec2(side * 3, 0);
						if (groundTiles.find(wallPos) != groundTiles.end()) wallExists = true;
						
					}

					if (wallExists) {
						auto physicsBody = PhysicsBody::createBox(Size(PLATFORM_WIDTH, PLATFORM_HEIGHT), PhysicsMaterial(0.1f, 0.0f, 0.0f));
						physicsBody->setDynamic(false);
						physicsBody->setCategoryBitmask(PLATFORM);
						physicsBody->setCollisionBitmask(PLAYER_BODY | ENEMY_BODY);
						physicsBody->setContactTestBitmask(PLAYER_BODY);

						auto node = Node::create();
						node->setPhysicsBody(physicsBody);

						Vec2 offset = Vec2(side * 1.5f, 0);
						Vec2 finalPos = currentPos + offset - origin + Vec2(0.5f, 0.5f);

						node->setPosition(finalPos * 24);
						corridorContainer->addChild(node);
						
						// Add platform texture
						auto platformSprite = Sprite::create("prison/platforms/woodenPlatform_0-=-0-=-.png");
						if (platformSprite) {
							platformSprite->setPosition(finalPos * 24);
							corridorContainer->addChild(platformSprite, -1);
						}
					}
				}
			}
		}
	}

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

		// Add corridor textures
		for (int row = 0; row < height; ++row) 
		{
			for (int col = 0; col < width; ++col) 
			{
				// Use background wall texture
				std::string tilePath = "prison/tiles/backWall_" + std::to_string(RandomHelper::random_int(0, 5)) + "-=-0-=-.png";
				auto tileSprite = Sprite::create(tilePath);
				if (tileSprite) {
					tileSprite->setContentSize(Size(24, 24));
					Vec2 tilePos = (Vec2(x + col, y + row) - origin) * 24 + Vec2(12, 12);
					tileSprite->setPosition(tilePos);
					corridorContainer->addChild(tileSprite, -1);
				}
				
				groundTiles.erase(Vec2(x + col, y + row));
			}
		}

	}

	return corridorContainer;
}

void GenMonster(Node* owner, cocos2d::Vector<MonsterLayer*>& _monsters, const std::vector<std::vector<PhysicsCategory>>& layerCategory)
{

	std::vector<Vec2> candidates;
	int height = layerCategory.size();
	int width = layerCategory[0].size();

	for (int y = 0; y < height - 2; ++y) 
	{
		for (int x = 0; x < width; ++x)
		{
			if (layerCategory[y][x] == PhysicsCategory::GROUND)
			{
				if (layerCategory[y + 1][x] == PhysicsCategory::AIR &&
					layerCategory[y + 2][x] == PhysicsCategory::AIR)
				{
					candidates.push_back(Vec2(x, y + 1));
				}
			}
		}
	}

	if (candidates.empty()) return;

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(candidates.begin(), candidates.end(), std::default_random_engine(seed));

	std::vector<Vec2> spawnPoints;
	const float minDistance = 5.0f; 
	
	int maxMonsters = std::max(1, (int)candidates.size() / 10);
	maxMonsters = std::min(maxMonsters, 4);

	for (const auto& pos : candidates)
	{
		if (spawnPoints.size() >= maxMonsters) break;

		bool tooClose = false;
		for (const auto& existing : spawnPoints)
		{
			
			if (pos.distance(existing) < minDistance)
			{
				tooClose = true;
				break;
			}
		}

		if (!tooClose)
		{
			spawnPoints.push_back(pos);
		}
	}

	for (const auto& sp : spawnPoints)
	{
		
		MonsterCategory type = static_cast<MonsterCategory>(RandomHelper::random_int(0, 2));

		Vec2 pixelPos = Vec2(sp.x * 24 + 12, sp.y * 24 + 12);

		auto monsterLayer = MonsterLayer::create(type, pixelPos);
		if (monsterLayer)
		{
			owner->addChild(monsterLayer, 100);
			_monsters.pushBack(monsterLayer); 
		}

	}

}

bool RoomNode::init(MapUnitData* data, cocos2d::Vector<MonsterLayer*>& _monsters)
{

	auto tmx = TMXTiledMap::create(data->name);
	this->addChild(tmx);
	this->setAnchorPoint(Vec2::ZERO);
	this->setPosition(data->obstacle.lowLeft * 24);

	tmx->setAnchorPoint(Vec2::ZERO);

	Size size = tmx->getMapSize();
	std::vector<std::vector<PhysicsCategory>> layerCategory(int(size.height), std::vector<PhysicsCategory>(int(size.width)));
	std::vector<std::vector<bool>> visited(int(size.height), std::vector<bool>(int(size.width), false));

	auto lnkLayer = tmx->getLayer("lnk");

	for (int x = 0; x < size.width; x++)
	{

		for (int y = 0; y < size.height; y++)
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

					if (tileType == "DOOR") layerCategory[size.height - y - 1][x] = PhysicsCategory::GROUND;

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
			if (pos.x < 0 || pos.x >= size.width || pos.y < 0 || pos.y >= size.height) continue;
			if (!visited[pos.y][pos.x] && layerCategory[pos.y][pos.x] == PhysicsCategory::GROUND)
			{

				visited[pos.y][pos.x] = true;
				layerCategory[pos.y][pos.x] = PhysicsCategory::AIR;
				q.push(pos);

			}

		}

		q.pop();

	}

	auto colLayer = tmx->getLayer("col");

	for (int x = 0; x < size.width; x++)
	{

		for (int y = 0; y < size.height; y++)
		{

			Vec2 tileCoord = Vec2(x, y);
			int gid = colLayer->getTileGIDAt(tileCoord);

			if (layerCategory[size.height - y - 1][x] == PhysicsCategory::GROUND) continue;

			if (gid)
			{

				Value properties = tmx->getPropertiesForGID(gid);
				if (!properties.isNull())
				{

					ValueMap propsMap = properties.asValueMap();
					std::string tileType = propsMap.at("cate").asString();

					if (tileType == "GROUND")
					{

						layerCategory[size.height - y - 1][x] = PhysicsCategory::GROUND;

					}
					else if (tileType == "LADDER")
					{
						layerCategory[size.height - y - 1][x] = PhysicsCategory::LADDER;
					}
					else if (tileType == "PLATFORM")
					{
						layerCategory[size.height - y - 1][x] = PhysicsCategory::PLATFORM;
					}
					else layerCategory[size.height - y - 1][x] = PhysicsCategory::AIR;

				}
				else layerCategory[size.height - y - 1][x] = PhysicsCategory::AIR;

			}
			else layerCategory[size.height - y - 1][x] = PhysicsCategory::AIR;

		}

	}

	if (data->chosenEntrance != -1)
	{

		Vec2 EntranceDir = data->entrance[data->chosenEntrance];
		
		if (EntranceDir.y == 0 || EntranceDir.y == data->obstacle.upperRight.y - data->obstacle.lowLeft.y)
		{

			int w = RandomHelper::random_int(0, 1), p = 0;
			if (w) p = 1;
			else p = -1;

			for (int x = 0; x < 3; x++)
			{
				
				if (layerCategory[EntranceDir.y][EntranceDir.x + x * p] == PhysicsCategory::AIR)
				{

					layerCategory[EntranceDir.y][EntranceDir.x + x * p] = PhysicsCategory::PLATFORM;
				
				}
			
			}

		}

	}

	if (data->chosenExit != -1)
	{

		Vec2 ExitDir = data->exit[data->chosenExit];

		if (ExitDir.y == 0 || ExitDir.y == data->obstacle.upperRight.y - data->obstacle.lowLeft.y)
		{

			int w = RandomHelper::random_int(0, 1), p = 0;
			if (w) p = 1;
			else p = -1;

			for (int x = 0; x < 3; x++)
			{

				if (layerCategory[ExitDir.y][ExitDir.x + x * p] == PhysicsCategory::AIR)
				{

					layerCategory[ExitDir.y][ExitDir.x + x * p] = PhysicsCategory::PLATFORM;

				}

			}

		}

	}

	for (int x = 0; x < size.width; x++)
	{

		for (int y = 1; y < size.height; y++)
		{

			if (layerCategory[y][x] == PhysicsCategory::LADDER && layerCategory[y - 1][x] == PhysicsCategory::GROUND)
			{
				layerCategory[y - 1][x] = PhysicsCategory::MIX;
			}

			if (layerCategory[y - 1][x] == PhysicsCategory::LADDER && layerCategory[y][x] == PhysicsCategory::GROUND)
			{
				layerCategory[y][x] = PhysicsCategory::MIX;
			}

			if (layerCategory[y][x] == PhysicsCategory::LADDER && layerCategory[y - 1][x] == PhysicsCategory::PLATFORM)
			{
				layerCategory[y - 1][x] = PhysicsCategory::MIX;
			}

			if (layerCategory[y - 1][x] == PhysicsCategory::LADDER && layerCategory[y][x] == PhysicsCategory::PLATFORM)
			{
				layerCategory[y][x] = PhysicsCategory::MIX;
			}

		}

	}

	GenBody(layerCategory, tmx);

	if (data->nextRoom.size())
	{

		std::vector<std::vector<Vec2>> paths;

		for (MapUnitData* nextRoom : data->nextRoom)
		{

			if (nextRoom->path.size()) paths.push_back(nextRoom->path);

		}

		if (paths.size())
		{

			Node* layer = GenCorridor(paths, data->obstacle.lowLeft);
			this->addChild(layer);

		}

	}

	auto objectGroup = tmx->getObjectGroup("markers");

	if (objectGroup) {
		auto& objects = objectGroup->getObjects();
		for (auto& obj : objects) {
			cocos2d::ValueMap& dict = obj.asValueMap();

			std::string type = dict["type"].asString();
			float x = dict["x"].asFloat();
			float y = dict["y"].asFloat();
			float w = dict["width"].asFloat();
			float h = dict["height"].asFloat();

			if (type == "ExitDoor")
			{
				auto node = Node::create();

				node->setName("ExitDoor");

				auto physicsBody = PhysicsBody::createBox(Size(w, h));
				physicsBody->setDynamic(false);       
				physicsBody->setGravityEnable(false); 

				physicsBody->setCategoryBitmask(INTERACTABLE);
				physicsBody->setCollisionBitmask(PLAYER_BODY); 
				physicsBody->setContactTestBitmask(PLAYER_BODY); 

				for (auto shape : physicsBody->getShapes()) {
					shape->setSensor(true);
				}

				node->setPhysicsBody(physicsBody);

				node->setPosition(x + w / 2, y + h / 2);

				tmx->addChild(node);
			}

			if (type == "REVIVE")
			{
				auto node = Node::create();
				node->setName("REVIVE");

				auto physicsBody = PhysicsBody::createBox(Size(w, h));
				physicsBody->setDynamic(false);
				physicsBody->setGravityEnable(false);

				physicsBody->setCategoryBitmask(INTERACTABLE);
				physicsBody->setCollisionBitmask(PLAYER_BODY);
				physicsBody->setContactTestBitmask(PLAYER_BODY);

				for (auto shape : physicsBody->getShapes()) {
					shape->setSensor(true);
				}

				node->setPhysicsBody(physicsBody);

				node->setPosition(x + w / 2, y + h / 2);

				tmx->addChild(node);
				
			}

			if (type == "GOODS")
			{

				CCLOG("FINDGOODS\n");

				WeaponNode* goodsNode = nullptr;

				int category = cocos2d::RandomHelper::random_int(0, 2);

				if (category == 0) 
				{
					int typeIdx = cocos2d::RandomHelper::random_int(0, 3);
					goodsNode = WeaponNode::createSword(static_cast<Sword::SwordType>(typeIdx), Vec2(x + w / 2, y + h / 2));
				}
				else if (category == 1) 
				{
					int typeIdx = cocos2d::RandomHelper::random_int(0, 2);
					goodsNode = WeaponNode::createBow(static_cast<Bow::BowType>(typeIdx), Vec2(x + w / 2, y + h / 2));
				}
				else 
				{
					int typeIdx = cocos2d::RandomHelper::random_int(0, 1);
					goodsNode = WeaponNode::createShield(static_cast<Shield::ShieldType>(typeIdx), Vec2(x + w / 2, y + h / 2));
				}

				if (goodsNode)
				{
					int price = cocos2d::RandomHelper::random_int(10, 30) * 100;

					goodsNode->setPrice(price);

					CCLOG("ADDNODE!\n");

					tmx->addChild(goodsNode, 100);
				}

			}

		}

	}

	if (data->roomtype == Type::combat)
	{
		GenMonster(this, _monsters, layerCategory);
	}

	// Hide collision layer
	if (colLayer) {
		colLayer->setVisible(false);
	}
	
	// Hide link layer
	if (lnkLayer) {
		lnkLayer->setVisible(false);
	}

	// Create TileRenderer for game textures
	_tileRenderer = TileRenderer::create("prison");
	if (_tileRenderer) {
		// Convert PhysicsCategory to TileType
		std::vector<std::vector<TileType>> tileData(int(size.height), std::vector<TileType>(int(size.width)));
		for (int y = 0; y < size.height; y++) {
			for (int x = 0; x < size.width; x++) {
				switch (layerCategory[y][x]) {
					case PhysicsCategory::GROUND:
						tileData[y][x] = TileType::GROUND;
						break;
					case PhysicsCategory::PLATFORM:
						tileData[y][x] = TileType::PLATFORM;
						break;
					case PhysicsCategory::LADDER:
						tileData[y][x] = TileType::LADDER;
						break;
					case PhysicsCategory::MIX:
						tileData[y][x] = TileType::GROUND;
						break;
					default:
						tileData[y][x] = TileType::AIR;
						break;
				}
			}
		}
		
		_tileRenderer->renderFromCollisionLayer(tmx, tileData);
		tmx->addChild(_tileRenderer, -100);
	}

	return true;

}