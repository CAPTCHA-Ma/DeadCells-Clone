#include "RoomNode.h"

RoomNode* RoomNode::create(MapUnitData* data)
{

	auto node = new RoomNode();
	if (node && node->init(data)) 
	{

		node->autorelease();
		return node;

	}

	CC_SAFE_DELETE(node);

	return nullptr;

}

void GenWall(const std::vector<std::vector<PhysicsCategory>>& layerCategory, TMXTiledMap* tmx)
{

	Size sz = Size(layerCategory[0].size(), layerCategory.size());
	std::vector<std::vector<bool>> visited(int(sz.height), std::vector<bool>(int(sz.width), false));

	for (int y = 0; y < sz.height; ++y)
	{

		for (int x = 0; x < sz.width; ++x)
		{

			if (layerCategory[y][x] != GROUND || visited[y][x]) continue;

			int width = 0;
			while (x + width < sz.width && layerCategory[y][x + width] == GROUND && !visited[y][x + width]) width++;

			int height = 1;
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

			for (int y0 = y; y0 < y + height; ++y0)
			{

				for (int x0 = x; x0 < x + width; ++x0) visited[y0][x0] = true;

			}

			auto physicsBody = PhysicsBody::createBox(Size(width * 24, height * 24), PhysicsMaterial(0.1f, 0.0f, 0.0f));
			physicsBody->setDynamic(false);
			physicsBody->setCategoryBitmask(GROUND);
			physicsBody->setCollisionBitmask(PLAYER_BODY | ENEMY_BODY);

			auto node = Node::create();
			node->setPhysicsBody(physicsBody);

			Vec2 pos = Vec2(x, y) * 24;
			pos += Vec2(width * 24 / 2, height * 24 / 2);

			node->setPosition(pos);

			tmx->addChild(node);

		}

	}

}

bool RoomNode::init(MapUnitData* data)
{

	auto tmx = TMXTiledMap::create(data->name);
	this->addChild(tmx);
	this->setPosition(data->obstacle.lowLeft * 24);

	tmx->setAnchorPoint(Vec2::ZERO);
	tmx->setPosition(data->obstacle.lowLeft * 24);

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

	GenWall(layerCategory, tmx);

	return true;

}