#include "Prison.h"
#include "Res/strings.h"
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>

struct AStarNode
{

	int g = 0, f = 0, h = 0;
	bool IsOccupied = false, extra = false;
	Vec2 last = Vec2::ZERO;

};
std::vector<std::vector<AStarNode>> AStarGraph;

struct compare
{

	bool operator()(Vec2 a, Vec2 b) 
	{

		return AStarGraph[a.x][a.y].f > AStarGraph[b.x][b.y].f;

	}

};

void Prison::SetupVisualScene()
{

	PrisonMapData = new SceneMapData; 
	PrisonScene = Scene::create();

	_debugDrawNode = DrawNode::create();
	auto visibleSize = Director::getInstance()->getVisibleSize();

	_debugDrawNode->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	PrisonScene->addChild(_debugDrawNode, 999);

	Director::getInstance()->replaceScene(PrisonScene);

}

void Prison::Draw()
{

	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {

		if (_debugDrawNode) {
			_debugDrawNode->clear();

			Vec2 move(-200, -100);

			for (MapUnitData* room : PrisonMapData->SceneMapDataUnit)
			{

				Color4F color = Color4F::WHITE;
				if (room->roomtype == Type::start) color = Color4F::GREEN;
				else if (room->roomtype == Type::combat) color = Color4F::RED;

				_debugDrawNode->drawRect(room->obstacle.lowLeft + move, room->obstacle.upperRight + move, color);


				/*if (room->preRoom) {
					_debugDrawNode->drawLine(room->center + move, room->preRoom->center + move, Color4F(1, 1, 1, 0.3f));
				}*/

				color = Color4F::RED;
				for (Vec2 entrance : room->entrance)
				{

					_debugDrawNode->drawRect(room->obstacle.lowLeft + entrance - Vec2(0.5, 0.5) + move, room->obstacle.lowLeft + entrance + Vec2(0.5, 0.5) + move, color);
					color = Color4F::BLUE;

				}

				color = Color4F::GREEN;
				for (Vec2 exit : room->exit)
				{

					_debugDrawNode->drawRect(room->obstacle.lowLeft + exit - Vec2(0.5, 0.5) + move, room->obstacle.lowLeft + exit + Vec2(0.5, 0.5) + move, color);
					color = Color4F::YELLOW;

				}

				if (room->path.size())
				{

					for (int i = 0; i < room->path.size() - 1; ++i)
					{

						Vec2 u = room->path[i];
						Vec2 v = room->path[i + 1];
						_debugDrawNode->drawLine(u + move, v + move, Color4F(1, 1, 1, 0.3f));

					}

				}

			}
		}

		});

}

void Prison::GenRoomPos()
{

	for (MapUnitData* roomData : PrisonMapData->SceneMapDataUnit)
	{

		if (roomData->preRoom == nullptr) continue;

		int tmpx = RandomHelper::random_int(0, 50),
			tmpy = RandomHelper::random_int(-50, 50);

		roomData->center = roomData->preRoom->center + Vec2(static_cast<float>(tmpx), static_cast<float>(tmpy));
		roomData->obstacle.lowLeft = Vec2(roomData->center.x - mDM->getRoomData(roomData->name)->width / 2 - CORR_WIDTH, roomData->center.y - mDM->getRoomData(roomData->name)->height / 2 - CORR_WIDTH);
		roomData->obstacle.upperRight = Vec2(roomData->center.x + mDM->getRoomData(roomData->name)->width / 2 + CORR_WIDTH, roomData->center.y + mDM->getRoomData(roomData->name)->height / 2 + CORR_WIDTH);

	}

	bool NotYet = true;
	int counter = 0;
	while (NotYet && counter <= MAX_TEMP)
	{

		++counter;

		NotYet = false;

		for (MapUnitData* room : PrisonMapData->SceneMapDataUnit)
		{

			if (room->preRoom)
			{

				float dist = room->center.distance(room->preRoom->center);
				float tmpx = (room->obstacle.upperRight.x - room->obstacle.lowLeft.x + room->preRoom->obstacle.upperRight.x - room->preRoom->obstacle.lowLeft.x) / 2,
					tmpy = (room->obstacle.upperRight.y - room->obstacle.lowLeft.y + room->preRoom->obstacle.upperRight.y - room->preRoom->obstacle.lowLeft.y) / 2;
				float maxDistance = std::sqrt(tmpx * tmpx + tmpy * tmpy);

				if (dist > maxDistance)
				{

					NotYet = true;

					Vec2 dir = room->preRoom->center - room->center;
					dir.normalize();
					dir *= PULL_FORCE;

					room->velocity += dir;
					room->preRoom->velocity -= dir;

				}

			}

		}

		for (int i = 0; i < PrisonMapData->SceneMapDataUnit.size(); ++i)
		{

			MapUnitData* roomA = PrisonMapData->SceneMapDataUnit[i];

			for (int j = i + 1; j < PrisonMapData->SceneMapDataUnit.size(); ++j)
			{

				MapUnitData* roomB = PrisonMapData->SceneMapDataUnit[j];

				if (IsRoomOverLap(roomA, roomB))
				{

					NotYet = true;

					Vec2 dir = roomB->center - roomA->center;
					if (dir.x == 0 && dir.y == 0) dir = Vec2(1, 0);
					dir.normalize();
					dir *= PUSH_FORCE;

					roomB->velocity += dir;
					roomA->velocity -= dir;

				}

			}

		}

		for (MapUnitData* room : PrisonMapData->SceneMapDataUnit) room->ChangePosition();

	}

	for (MapUnitData* room : PrisonMapData->SceneMapDataUnit)
	{

		room->obstacle.lowLeft += Vec2(CORR_WIDTH, CORR_WIDTH);
		room->obstacle.upperRight -= Vec2(CORR_WIDTH, CORR_WIDTH);

	}

}

void CalH(Vec2 startPos, Vec2 endPos)
{

	AStarGraph[startPos.x][startPos.y].h = std::abs(endPos.x - startPos.x) + std::abs(endPos.y - startPos.y);

	if (AStarGraph[startPos.x][startPos.y].extra) AStarGraph[startPos.x][startPos.y].h -= 1;

}

bool IsOut(Vec2 pos)
{

	int x = AStarGraph.size(),
		y = AStarGraph[0].size();

	if (pos.x <= 0 || pos.x >= x || pos.y <= 0 || pos.y >= y) return true;
	else return false;

}

bool FindPath(std::vector<std::vector<int>>& mapArray, Vec2 startPos, Vec2 endPos, std::vector<Vec2>& path)
{
	
	std::priority_queue<Vec2, std::vector<Vec2>, compare> frontier;
	AStarGraph[startPos.x][startPos.y].IsOccupied = true;
	AStarGraph[startPos.x][startPos.y].g = 0;
	CalH(startPos, endPos);
	AStarGraph[startPos.x][startPos.y].f = AStarGraph[startPos.x][startPos.y].g + AStarGraph[startPos.x][startPos.y].h;
	frontier.push(startPos);

	Vec2 dir[4] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };

	int counter = 0;
	while (frontier.size())
	{

		Vec2 front = frontier.top();
		frontier.pop();

		for (int i = 0; i < 4; ++i)
		{

			Vec2 pos = front + dir[i];

			if (pos == endPos)
			{

				Vec2 index = pos;
				AStarGraph[pos.x][pos.y].last = front;

				while (index != startPos)
				{

					path.push_back(index);
					index = AStarGraph[index.x][index.y].last;

				}

				return true;

			}

			if (IsOut(pos)) continue;

			if (AStarGraph[pos.x][pos.y].IsOccupied || (mapArray[pos.x][pos.y] == BLOCK_WEIGHT)) continue;

			AStarGraph[pos.x][pos.y].g = AStarGraph[front.x][front.y].g + 1;
			CalH(pos, endPos);
			AStarGraph[pos.x][pos.y].f = AStarGraph[pos.x][pos.y].g + AStarGraph[pos.x][pos.y].h;
			AStarGraph[pos.x][pos.y].IsOccupied = true;
			AStarGraph[pos.x][pos.y].last = front;

			frontier.push(pos);

		}

	}

	return false;

}

bool Prison::InitPrisonData()
{

	PrisonMapData = new SceneMapData;
	mDM = MapDataManager::getInstance();

	int AllNum = RandomHelper::random_int(12, 15),
		eliteNum = RandomHelper::random_int(0, 2),
		treasureNum = RandomHelper::random_int(1, 2),
		traderNum = 1,
		exitNum = 1;
	int specialNum = eliteNum + treasureNum + traderNum + exitNum,
		tripleNum = RandomHelper::random_int(1, (specialNum - 1) / 2),
		doubleNum = specialNum - tripleNum * 2 - 1,
		singleNum = AllNum - specialNum - doubleNum - tripleNum;
	int sonNum[4] = { specialNum, singleNum, doubleNum, tripleNum }, specialRoomNum[4] = { eliteNum, treasureNum, traderNum, exitNum };

	std::queue<MapUnitData*> roomQueue;
	MapUnitData* startRoomData = new MapUnitData;
	startRoomData->roomtype = Type::start;
	startRoomData->name = "PrisonStart.tmx";
	startRoomData->preRoom = nullptr;
	PrisonMapData->SceneMapDataUnit.push_back(startRoomData);
	roomQueue.push(startRoomData);

	while (!roomQueue.empty())
	{

		MapUnitData* room = roomQueue.front();
		roomQueue.pop();

		std::vector<int> sonNumPool;
		if ((!roomQueue.empty()) || !(sonNum[1] + sonNum[2] + sonNum[3])) sonNumPool.insert(sonNumPool.end(), sonNum[0], 0);
		if (sonNum[1]) sonNumPool.insert(sonNumPool.end(), sonNum[1], 1);
		if (sonNum[2]) sonNumPool.insert(sonNumPool.end(), sonNum[2], 2);
		if (sonNum[3]) sonNumPool.insert(sonNumPool.end(), sonNum[3], 3);
		
		int chosenSonNum = sonNumPool[RandomHelper::random_int(0, int(sonNumPool.size() - 1))];
		--sonNum[chosenSonNum];

		if (!chosenSonNum)
		{

			std::vector<int> specialRoomPool;

			for (int i = 0; i < 3; ++i)
			{

				if (specialRoomNum[i]) specialRoomPool.insert(specialRoomPool.end(), specialRoomNum[i], i);

			}

			if (specialRoomPool.size())
			{

				int chosenSpecialRoomNum = specialRoomPool[RandomHelper::random_int(0, int(specialRoomPool.size() - 1))];
				--specialRoomNum[chosenSpecialRoomNum];
				room->roomtype = static_cast<Type>(chosenSpecialRoomNum);

			}
			else room->roomtype = Type::exit;
			
			switch (room->roomtype)
			{

			case Type::elite:
				room->name = "PrisonElite.tmx";
				break;
			case Type::treasure:
				room->name = "PrisonTreasure.tmx";
				break;
			case Type::trader:
				room->name = "PrisonTrader.tmx";
				break;
			case Type::exit:
				room->name = "PrisonExit.tmx";
				break;

			}

		}
		else
		{

			if (room != startRoomData)
			{

				room->roomtype = Type::combat;
				int combatRoomIndex = RandomHelper::random_int(1, 60);
				room->name = "Pr" + std::to_string(combatRoomIndex) + ".tmx";

			}

			while (chosenSonNum--)
			{

				MapUnitData* sonRoomData = new MapUnitData;
				sonRoomData->preRoom = room;
				room->nextRoom.push_back(sonRoomData);
				roomQueue.push(sonRoomData);
				PrisonMapData->SceneMapDataUnit.push_back(sonRoomData);

			}

		}

	}

	Sleep(1000);

RESTART:

	while (true)
	{

		MapArray.clear();
		AStarGraph.clear();
		startRoomData->obstacle.lowLeft = Vec2(-CORR_WIDTH, -CORR_WIDTH);
		startRoomData->obstacle.upperRight = Vec2(static_cast<float>(mDM->getRoomData(startRoomData->name)->width + CORR_WIDTH), static_cast<float>(mDM->getRoomData(startRoomData->name)->height + CORR_WIDTH));
		startRoomData->center = Vec2(static_cast<float>(mDM->getRoomData(startRoomData->name)->width / 2), static_cast<float>(mDM->getRoomData(startRoomData->name)->height / 2));
		GenRoomPos();

		for (int i = 0; i < PrisonMapData->SceneMapDataUnit.size(); ++i)
		{

			MapUnitData* roomA = PrisonMapData->SceneMapDataUnit[i];

			if (!roomA->nextRoom.size()) continue;

			for (MapUnitData* sonroomA : roomA->nextRoom)
			{

				for (int j = i + 1; j < PrisonMapData->SceneMapDataUnit.size(); ++j)
				{

					MapUnitData* roomB = PrisonMapData->SceneMapDataUnit[j];
					if (!roomB->nextRoom.size() || roomB == sonroomA) continue;

					for (MapUnitData* sonroomB : roomB->nextRoom)
					{

						float s, t;
						bool result = Vec2::isLineIntersect(roomA->center, sonroomA->center, roomB->center, sonroomB->center, &s, &t);

						if (result && s > 0 && s < 1 && t > 0 && t < 1) goto RESTART;

					}

				}

			}

		}

		int minLowLeftX = INT_MAX, minLowLeftY = INT_MAX, maxUpperRightX = INT_MIN, maxUpperRightY = INT_MIN;

		for (MapUnitData* room : PrisonMapData->SceneMapDataUnit)
		{

			room->obstacle.lowLeft = Vec2(int(room->obstacle.lowLeft.x), int(room->obstacle.lowLeft.y));
			room->obstacle.upperRight = Vec2(int(room->obstacle.upperRight.x), int(room->obstacle.upperRight.y));
			room->center = Vec2(int(room->center.x), int(room->center.y));

			for (Vec2 u : mDM->getRoomData(room->name)->entrances) room->entrance.push_back(u);
			for (Vec2 u : mDM->getRoomData(room->name)->exits) room->exit.push_back(u);

			if (room->obstacle.lowLeft.x < minLowLeftX) minLowLeftX = room->obstacle.lowLeft.x;
			if (room->obstacle.lowLeft.y < minLowLeftY) minLowLeftY = room->obstacle.lowLeft.y;
			if (room->obstacle.upperRight.x > maxUpperRightX) maxUpperRightX = room->obstacle.upperRight.x;
			if (room->obstacle.upperRight.y > maxUpperRightY) maxUpperRightY = room->obstacle.upperRight.y;

		}

		Vec2 moveVec = Vec2(-minLowLeftX + 5 * CORR_WIDTH, -minLowLeftY + 5 * CORR_WIDTH);
		for (MapUnitData* room : PrisonMapData->SceneMapDataUnit)
		{

			room->velocity = moveVec;
			room->ChangePosition();

		}

		MapArray.assign(maxUpperRightX - minLowLeftX + 10 * CORR_WIDTH, std::vector<int>(maxUpperRightY - minLowLeftY + 10 * CORR_WIDTH, 0));
		AStarGraph.assign(maxUpperRightX - minLowLeftX + 10 * CORR_WIDTH, std::vector<AStarNode>(maxUpperRightY - minLowLeftY + 10 * CORR_WIDTH));

		for (MapUnitData* room : PrisonMapData->SceneMapDataUnit)
		{

			Vec2& ll = room->obstacle.lowLeft;
			Vec2& ur = room->obstacle.upperRight;

			for (int x = ll.x - 4 * CORR_WIDTH; x <= ur.x + 4 * CORR_WIDTH; ++x)
			{

				for (int y = ll.y - 4 * CORR_WIDTH; y <= ur.y + 4 * CORR_WIDTH; ++y)
				{

					if (x >= ll.x && x <= ur.x && y >= ll.y && y <= ur.y) MapArray[x][y] = BLOCK_WEIGHT;
					else MapArray[x][y] = SURROUND_WEIGHT;

				}

			}

			if (room->preRoom)
			{

				Vec2 parent = room->preRoom->center, self = room->obstacle.lowLeft;
				std::sort(room->entrance.begin(), room->entrance.end(), [&](Vec2 door1, Vec2 door2) {

					return (self + door1).distance(parent) < (self + door2).distance(parent);

					});

			}

			if (room->nextRoom.size())
			{

				Vec2 self = room->obstacle.lowLeft;
				std::sort(room->exit.begin(), room->exit.end(), [&](Vec2 door1, Vec2 door2) {

					float dis1 = 0, dis2 = 0;

					for (MapUnitData* sonRoom : room->nextRoom)
					{

						dis1 += (self + door1).distance(sonRoom->center);
						dis2 += (self + door2).distance(sonRoom->center);

					}

					return dis1 < dis2;

					});

			}

		}

		CCLOG("AGAIN!\n");

		for (MapUnitData* room : PrisonMapData->SceneMapDataUnit)
		{

			if (!room->nextRoom.size()) continue;

			bool IsRoomConnected = false;

			for (Vec2 exit : room->exit)
			{

				Vec2 start = room->obstacle.lowLeft + exit;
				bool sit = true;

				for (MapUnitData* sonRoom : room->nextRoom)
				{

					bool IsPathConnected = false;

					for (Vec2 entrance : sonRoom->entrance)
					{

						
						Vec2 end = sonRoom->obstacle.lowLeft + entrance;
						if (FindPath(MapArray, start, end, sonRoom->path))
						{

							CCLOG("%f %f %f %f\n", start.x, start.y, end.x, end.y);
							IsPathConnected = true;
							break;

						}

					}

					for (auto& row : AStarGraph)
					{

						for (auto& node : row)
						{

							if (node.IsOccupied)
							{

								node.IsOccupied = false;
								node.last = Vec2::ZERO;

							}

						}

					}

					if (!IsPathConnected)
					{

						CCLOG("?!\n");

						for (MapUnitData* sonRoom : room->nextRoom) sonRoom->path.clear();

						sit = false;
						break;

					}

					for (Vec2 index : sonRoom->path) AStarGraph[index.x][index.y].extra = true;

				}

				for (auto& row : AStarGraph)
				{

					for (auto& node : row)
					{

						if (node.extra) node.extra = false;

					}

				}

				if (sit)
				{

					IsRoomConnected = true;
					break;

				}

			}

			if (!IsRoomConnected)
			{

				goto RESTART;

			}
			
		}

		break;

	}

	Draw();

	return true;

}	

bool Prison::RenderPrisonScene()
{

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto prisonMap = TMXTiledMap::create("room/Prison/Pr2.tmx");
	prisonMap->setAnchorPoint(Vec2(0, origin.y / 2));
	prisonMap->setPosition(Vec2(origin.x, origin.y));

	PrisonScene = Scene::create();
	PrisonScene->addChild(prisonMap, 0);
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, PrisonScene));

	return true;

}