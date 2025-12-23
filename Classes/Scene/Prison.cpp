#include "Prison.h"
#include "Res/strings.h"
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>

void PrisonMapGen::GenRoomPos()
{

	for (MapUnitData* roomData : _rooms)
	{

		if (roomData->preRoom == nullptr) continue;

		int tmpx = RandomHelper::random_int(0, 100),
			tmpy = RandomHelper::random_int(-12, 12);

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

		for (MapUnitData* room : _rooms)
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

		for (int i = 0; i < _rooms.size(); ++i)
		{

			MapUnitData* roomA = _rooms[i];

			for (int j = i + 1; j < _rooms.size(); ++j)
			{

				MapUnitData* roomB = _rooms[j];

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

		for (MapUnitData* room : _rooms) room->ChangePosition();

	}

	for (MapUnitData* room : _rooms)
	{

		room->obstacle.lowLeft += Vec2(CORR_WIDTH, CORR_WIDTH);
		room->obstacle.upperRight -= Vec2(CORR_WIDTH, CORR_WIDTH);

	}

}

void PrisonMapGen::Generate()
{

	mDM = MapDataManager::getInstance();
	std::vector<std::vector<int>> MapArray;
	std::vector<std::vector<AStarNode>> AStarGraph;

GenRoom:

	for (MapUnitData* room : _rooms) delete room;
	_rooms.clear();

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
	_rooms.push_back(startRoomData);
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
				_rooms.push_back(sonRoomData);

			}

		}

	}

	int counter = 0;

GenPos:

	++counter;
	CCLOG("%d", counter);
	if (counter == MAX_TEMP)
	{

		counter = 0;
		goto GenRoom;

	}

	MapArray.clear();
	AStarGraph.clear();
	startRoomData->obstacle.lowLeft = Vec2(-CORR_WIDTH, -CORR_WIDTH);
	startRoomData->obstacle.upperRight = Vec2(static_cast<float>(mDM->getRoomData(startRoomData->name)->width - 1 + CORR_WIDTH), static_cast<float>(mDM->getRoomData(startRoomData->name)->height - 1 + CORR_WIDTH));
	startRoomData->center = Vec2(static_cast<float>(mDM->getRoomData(startRoomData->name)->width / 2), static_cast<float>(mDM->getRoomData(startRoomData->name)->height / 2));
	GenRoomPos();

	for (int i = 0; i < _rooms.size(); ++i)
	{

		MapUnitData* roomA = _rooms[i];

		if (!roomA->nextRoom.size()) continue;

		for (MapUnitData* sonroomA : roomA->nextRoom)
		{

			for (int j = i + 1; j < _rooms.size(); ++j)
			{

				MapUnitData* roomB = _rooms[j];
				if (!roomB->nextRoom.size() || roomB == sonroomA) continue;

				for (MapUnitData* sonroomB : roomB->nextRoom)
				{

					float s, t;
					bool result = Vec2::isLineIntersect(roomA->center, sonroomA->center, roomB->center, sonroomB->center, &s, &t);

					if (result && s > 0 && s < 1 && t > 0 && t < 1) goto GenPos;

				}

			}

		}

	}

	int minLowLeftX = INT_MAX, minLowLeftY = INT_MAX, maxUpperRightX = INT_MIN, maxUpperRightY = INT_MIN;

	for (MapUnitData* room : _rooms)
	{

		room->obstacle.lowLeft = Vec2(int(room->obstacle.lowLeft.x), int(room->obstacle.lowLeft.y));
		room->obstacle.upperRight = room->obstacle.lowLeft + Vec2(mDM->getRoomData(room->name)->width - 1, mDM->getRoomData(room->name)->height - 1);
		room->center = (room->obstacle.lowLeft + room->obstacle.upperRight) / 2;

		for (Vec2 u : mDM->getRoomData(room->name)->entrances) room->entrance.push_back(u);
		for (Vec2 u : mDM->getRoomData(room->name)->exits) room->exit.push_back(u);

		if (room->obstacle.lowLeft.x < minLowLeftX) minLowLeftX = room->obstacle.lowLeft.x;
		if (room->obstacle.lowLeft.y < minLowLeftY) minLowLeftY = room->obstacle.lowLeft.y;
		if (room->obstacle.upperRight.x > maxUpperRightX) maxUpperRightX = room->obstacle.upperRight.x;
		if (room->obstacle.upperRight.y > maxUpperRightY) maxUpperRightY = room->obstacle.upperRight.y;

	}

	Vec2 moveVec = Vec2(-minLowLeftX + 5 * CORR_WIDTH, -minLowLeftY + 5 * CORR_WIDTH);
	for (MapUnitData* room : _rooms)
	{

		room->velocity = moveVec;
		room->ChangePosition();

	}

	MapArray.assign(maxUpperRightX - minLowLeftX + 10 * CORR_WIDTH, std::vector<int>(maxUpperRightY - minLowLeftY + 10 * CORR_WIDTH, 0));
	AStarGraph.assign(maxUpperRightX - minLowLeftX + 10 * CORR_WIDTH, std::vector<AStarNode>(maxUpperRightY - minLowLeftY + 10 * CORR_WIDTH));

	for (MapUnitData* room : _rooms)
	{

		Vec2& ll = room->obstacle.lowLeft;
		Vec2& ur = room->obstacle.upperRight;

		for (int x = int(ll.x); x <= int(ur.x); ++x)
		{

			for (int y = int(ll.y); y <= int(ur.y); ++y)
			{

				MapArray[x][y] = BLOCK_WEIGHT;

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

	for (MapUnitData* room : _rooms)
	{

		if (!room->nextRoom.size()) continue;

		bool IsRoomConnected = false;

		for (int j = 0; j < room->exit.size(); ++j)
		{

			Vec2 exit = room->exit[j];
			bool IsExitCorrected = true;
			Vec2 start = room->obstacle.lowLeft + exit;
			std::vector<Vec2> path[3];
			int entIndex[3];

			for (int i = 0; i < room->nextRoom.size(); ++i)
			{

				MapUnitData* sonRoom = room->nextRoom[i];
				bool IsPathConnected = false;
				path[i].clear();

				for (int k = 0; k < sonRoom->entrance.size(); ++k)
				{

					Vec2 entrance = sonRoom->entrance[k];
					Vec2 end = sonRoom->obstacle.lowLeft + entrance;
					std::vector<Vec2> tmppath;

					bool result = FindPath(MapArray, AStarGraph, start, end, tmppath);

					if (tmppath.size())
					{

						if (!path[i].size() || (tmppath.size() < path[i].size()))
						{

							path[i] = tmppath;
							entIndex[i] = k;

						}

					}

					for (auto& row : AStarGraph)
					{

						for (auto& col : row)
						{

							if (col.IsOccupied) col.IsOccupied = false;

						}

					}

					if (result) IsPathConnected = true;

				}

				if (!IsPathConnected)
				{

					IsExitCorrected = false;

				}
				else
				{

					for (Vec2 point : path[i]) AStarGraph[point.x][point.y].extra = true;

				}

			}

			for (auto& row : AStarGraph)
			{

				for (auto& col : row)
				{

					if (col.extra) col.extra = false;

				}

			}

			if (IsExitCorrected)
			{

				if (!IsRoomConnected)
				{

					IsRoomConnected = true;
					for (int i = 0; i < room->nextRoom.size(); ++i)
					{

						room->nextRoom[i]->path = path[i];
						room->nextRoom[i]->chosenEntrance = entIndex[i];

					}
					room->chosenExit = j;

				}
				else
				{

					int prelen = 0, nowlen = 0;
					for (int i = 0; i < room->nextRoom.size(); ++i)
					{

						prelen += room->nextRoom[i]->path.size();
						nowlen += path[i].size();

					}

					if (nowlen < prelen)
					{

						for (int i = 0; i < room->nextRoom.size(); ++i)
						{

							room->nextRoom[i]->path = path[i];
							room->nextRoom[i]->chosenEntrance = entIndex[i];

						}
						room->chosenExit = j;

					}

				}

			}

		}

		if (!IsRoomConnected) goto GenPos;
		else
		{

			for (MapUnitData* sonRoom : room->nextRoom)
			{

				for (Vec2 point : sonRoom->path)
				{

					MapArray[point.x][point.y] = BLOCK_WEIGHT;

				}

			}

		}

	}

}

bool PrisonMapGen::FindPath(std::vector<std::vector<int>>& mapArray, std::vector<std::vector<AStarNode>>& AStarGraph, Vec2 startPos, Vec2 endPos, std::vector<Vec2>& path)
{

	compare cmp(AStarGraph);
	std::priority_queue<Vec2, std::vector<Vec2>, compare> frontier(cmp);
	AStarGraph[startPos.x][startPos.y].IsOccupied = true;
	AStarGraph[startPos.x][startPos.y].g = 0;
	CalH(startPos, endPos, AStarGraph);
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

			if (IsOut(pos, AStarGraph)) continue;

			if (AStarGraph[pos.x][pos.y].IsOccupied || (mapArray[pos.x][pos.y] == BLOCK_WEIGHT)) continue;

			bool IsWidthEnough = true;
			for (int j = 0; j <= 1; ++j)
			{

				Vec2 point = pos;

				for (int k = 0; k <= 1; ++k)
				{

					point += dir[(1 ^ (i / 2)) * 2 + j];

					if (IsOut(point, AStarGraph))
					{

						IsWidthEnough = false;
						break;

					}

					if (mapArray[point.x][point.y] == BLOCK_WEIGHT)
					{

						IsWidthEnough = false;
						break;

					}

				}

			}

			if (!IsWidthEnough) continue;

			if (AStarGraph[front.x][front.y].lastDir != -1 && i != AStarGraph[front.x][front.y].lastDir)
			{

				for (int j = 1; j <= 2; ++j)
				{

					Vec2 point = pos + dir[AStarGraph[front.x][front.y].lastDir] * j;

					for (int k = 0; k <= 2; ++k)
					{

						point += dir[i ^ 1];

						if (IsOut(point, AStarGraph))
						{

							IsWidthEnough = false;
							break;

						}

						if (mapArray[point.x][point.y] == BLOCK_WEIGHT)
						{

							IsWidthEnough = false;
							break;

						}

					}

				}

				if (!IsWidthEnough) continue;

			}

			AStarGraph[pos.x][pos.y].g = AStarGraph[front.x][front.y].g + mapArray[pos.x][pos.y];
			if (i == 2 || i == 3)
			{

				if (AStarGraph[front.x][front.y].lastDir != i) AStarGraph[pos.x][pos.y].height = 0;
				else AStarGraph[pos.x][pos.y].height = AStarGraph[front.x][front.y].height + 1;
				if (!AStarGraph[pos.x][pos.y].extra) AStarGraph[pos.x][pos.y].g += 1 + AStarGraph[pos.x][pos.y].height;

			}
			else
			{

				AStarGraph[pos.x][pos.y].height = 0;
				if (!AStarGraph[pos.x][pos.y].extra) AStarGraph[pos.x][pos.y].g += 1;

			}
			AStarGraph[pos.x][pos.y].lastDir = i;
			if (AStarGraph[front.x][front.y].lastDir != -1 && AStarGraph[front.x][front.y].lastDir != AStarGraph[pos.x][pos.y].lastDir) AStarGraph[pos.x][pos.y].g += TURN_PUNISH;
			CalH(pos, endPos, AStarGraph);
			AStarGraph[pos.x][pos.y].f = AStarGraph[pos.x][pos.y].g + AStarGraph[pos.x][pos.y].h;
			AStarGraph[pos.x][pos.y].IsOccupied = true;
			AStarGraph[pos.x][pos.y].last = front;

			frontier.push(pos);

		}

	}

	return false;

}