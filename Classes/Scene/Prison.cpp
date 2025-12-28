#include "Prison.h"
#include "Res/strings.h"
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>

// 生成房间位置
void PrisonMapGen::GenRoomPos()
{

	// 初始化房间位置 便于后续力导向算法迭代
	for (MapUnitData* roomData : _rooms)
	{

		if (roomData->pre_room == nullptr) continue;

		int tmpx = RandomHelper::random_int(0, 100),
			tmpy = RandomHelper::random_int(-12, 12);

		roomData->center = roomData->pre_room->center + Vec2(static_cast<float>(tmpx), static_cast<float>(tmpy));
		roomData->obstacle.low_left = Vec2(roomData->center.x - mDM->getRoomData(roomData->name)->width / 2 - CORR_WIDTH, roomData->center.y - mDM->getRoomData(roomData->name)->height / 2 - CORR_WIDTH);
		roomData->obstacle.upper_right = Vec2(roomData->center.x + mDM->getRoomData(roomData->name)->width / 2 + CORR_WIDTH, roomData->center.y + mDM->getRoomData(roomData->name)->height / 2 + CORR_WIDTH);

	}

	// 力导向算法迭代调整房间位置
	bool NotYet = true;
	int counter = 0;
	while (NotYet && counter <= MAX_POS_TEMP)
	{

		++counter;

		NotYet = false;

		// 计算受力

		// 上下级房间间的拉力
		for (MapUnitData* room : _rooms)
		{

			if (room->pre_room)
			{

				float dist = room->center.distance(room->pre_room->center);
				float tmpx = (room->obstacle.upper_right.x - room->obstacle.low_left.x + room->pre_room->obstacle.upper_right.x - room->pre_room->obstacle.low_left.x) / 2,
					tmpy = (room->obstacle.upper_right.y - room->obstacle.low_left.y + room->pre_room->obstacle.upper_right.y - room->pre_room->obstacle.low_left.y) / 2;
				float maxDistance = std::sqrt(tmpx * tmpx + tmpy * tmpy);

				if (dist > maxDistance)
				{

					NotYet = true;

					Vec2 dir = room->pre_room->center - room->center;
					dir.normalize();
					dir *= PULL_FORCE;

					room->velocity += dir;
					room->pre_room->velocity -= dir;

				}

			}

		}

        // 重叠房间间斥力
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

        // 位置调整
		for (MapUnitData* room : _rooms) room->ChangePosition();

	}

	// 收缩房间碰撞箱 去除预留的走廊空间
	for (MapUnitData* room : _rooms)
	{

		room->obstacle.low_left += Vec2(CORR_WIDTH, CORR_WIDTH);
		room->obstacle.upper_right -= Vec2(CORR_WIDTH, CORR_WIDTH);

	}

}

// 地图生成函数
void PrisonMapGen::Generate()
{
    mDM = MapDataManager::getInstance();
    std::vector<std::vector<int>> MapArray;
    std::vector<std::vector<AStarNode>> AStarGraph;

	// 不断尝试生成地图直到成功
    while (true)
    {
        for (MapUnitData* room : _rooms) delete room;
        _rooms.clear();

        // 确定房间组成
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
        startRoomData->room_type = Type::start;
        startRoomData->name = "PrisonStart.tmx";
        startRoomData->pre_room = nullptr;
        _rooms.push_back(startRoomData);
        roomQueue.push(startRoomData);

		// 生成房间树结构
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
                    room->room_type = static_cast<Type>(chosenSpecialRoomNum);
                }
                else room->room_type = Type::exit;

                switch (room->room_type)
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
                    room->room_type = Type::combat;
                    int combatRoomIndex = RandomHelper::random_int(1, 60);
                    room->name = "Pr" + std::to_string(combatRoomIndex) + ".tmx";
                }

                while (chosenSonNum--)
                {
                    MapUnitData* sonRoomData = new MapUnitData;
                    sonRoomData->pre_room = room;
                    room->next_room.push_back(sonRoomData);
                    roomQueue.push(sonRoomData);
                    _rooms.push_back(sonRoomData);
                }
            }
        }


		// 确定房间位置、生成走廊
        int counter = 0;

        while (true)
        {

			// 多次生成不成功则重新生成房间结构
            ++counter;
            CCLOG("%d", counter);
            if (counter == MAX_TEMP)
            {
                counter = 0;
                break; 
            }

			// 位置初始化
            MapArray.clear();
            AStarGraph.clear();
            startRoomData->obstacle.low_left = Vec2(-CORR_WIDTH, -CORR_WIDTH);
            startRoomData->obstacle.upper_right = Vec2(static_cast<float>(mDM->getRoomData(startRoomData->name)->width - 1 + CORR_WIDTH), static_cast<float>(mDM->getRoomData(startRoomData->name)->height - 1 + CORR_WIDTH));
            startRoomData->center = Vec2(static_cast<float>(mDM->getRoomData(startRoomData->name)->width / 2), static_cast<float>(mDM->getRoomData(startRoomData->name)->height / 2));
            GenRoomPos();


			// 判断房间连线是否相交
            bool needsRetryPos = false; 

            for (int i = 0; i < _rooms.size(); ++i)
            {
                MapUnitData* roomA = _rooms[i];
                if (!roomA->next_room.size()) continue;

                for (MapUnitData* sonroomA : roomA->next_room)
                {
                    for (int j = i + 1; j < _rooms.size(); ++j)
                    {
                        MapUnitData* roomB = _rooms[j];
                        if (!roomB->next_room.size() || roomB == sonroomA) continue;

                        for (MapUnitData* sonroomB : roomB->next_room)
                        {
                            float s, t;
                            bool result = Vec2::isLineIntersect(roomA->center, sonroomA->center, roomB->center, sonroomB->center, &s, &t);

                            if (result && s > 0 && s < 1 && t > 0 && t < 1) {
                                needsRetryPos = true;
                                break;
                            }
                        }
                        if (needsRetryPos) break;
                    }
                    if (needsRetryPos) break;
                }
                if (needsRetryPos) break;
            }

            if (needsRetryPos) continue;

            // 确定地图大致尺寸
            int minLowLeftX = INT_MAX, minLowLeftY = INT_MAX, maxUpperRightX = INT_MIN, maxUpperRightY = INT_MIN;

            for (MapUnitData* room : _rooms)
            {
                room->obstacle.low_left = Vec2(int(room->obstacle.low_left.x), int(room->obstacle.low_left.y));
                room->obstacle.upper_right = room->obstacle.low_left + Vec2(mDM->getRoomData(room->name)->width - 1, mDM->getRoomData(room->name)->height - 1);
                room->center = (room->obstacle.low_left + room->obstacle.upper_right) / 2;

                for (Vec2 u : mDM->getRoomData(room->name)->entrances) room->entrance.push_back(u);
                for (Vec2 u : mDM->getRoomData(room->name)->exits) room->exit.push_back(u);

                if (room->obstacle.low_left.x < minLowLeftX) minLowLeftX = room->obstacle.low_left.x;
                if (room->obstacle.low_left.y < minLowLeftY) minLowLeftY = room->obstacle.low_left.y;
                if (room->obstacle.upper_right.x > maxUpperRightX) maxUpperRightX = room->obstacle.upper_right.x;
                if (room->obstacle.upper_right.y > maxUpperRightY) maxUpperRightY = room->obstacle.upper_right.y;
            }

            Vec2 moveVec = Vec2(-minLowLeftX + 5 * CORR_WIDTH, -minLowLeftY + 5 * CORR_WIDTH);
            for (MapUnitData* room : _rooms)
            {
                room->velocity = moveVec;
                room->ChangePosition();
            }

            MapArray.assign(maxUpperRightX - minLowLeftX + 10 * CORR_WIDTH, std::vector<int>(maxUpperRightY - minLowLeftY + 10 * CORR_WIDTH, 0));
            AStarGraph.assign(maxUpperRightX - minLowLeftX + 10 * CORR_WIDTH, std::vector<AStarNode>(maxUpperRightY - minLowLeftY + 10 * CORR_WIDTH));

			// 在地图数组中标记房间位置及排序房间出入口
            for (MapUnitData* room : _rooms)
            {
                Vec2& ll = room->obstacle.low_left;
                Vec2& ur = room->obstacle.upper_right;

                for (int x = int(ll.x); x <= int(ur.x); ++x)
                {
                    for (int y = int(ll.y); y <= int(ur.y); ++y)
                    {
                        MapArray[x][y] = BLOCK_WEIGHT;
                    }
                }

                if (room->pre_room)
                {
                    Vec2 parent = room->pre_room->center, self = room->obstacle.low_left;
                    std::sort(room->entrance.begin(), room->entrance.end(), [&](Vec2 door1, Vec2 door2) {
                        return (self + door1).distance(parent) < (self + door2).distance(parent);
                        });
                }

                if (room->next_room.size())
                {
                    Vec2 self = room->obstacle.low_left;
                    std::sort(room->exit.begin(), room->exit.end(), [&](Vec2 door1, Vec2 door2) {
                        float dis1 = 0, dis2 = 0;
                        for (MapUnitData* sonRoom : room->next_room)
                        {
                            dis1 += (self + door1).distance(sonRoom->center);
                            dis2 += (self + door2).distance(sonRoom->center);
                        }
                        return dis1 < dis2;
                        });
                }
            }

            // 试图生成走廊并寻找最优解
            bool IsRoomConnectedGlobal = true;

            for (MapUnitData* room : _rooms)
            {
                if (!room->next_room.size()) continue;

                bool IsRoomConnected = false;

				// 枚举房间所有出口
                for (int j = 0; j < room->exit.size(); ++j)
                {
                    Vec2 exit = room->exit[j];
                    bool IsExitCorrected = true;
                    Vec2 start = room->obstacle.low_left + exit;
                    std::vector<Vec2> path[3];
                    int entIndex[3];

					// 为每个子房间寻找最优入口
                    for (int i = 0; i < room->next_room.size(); ++i)
                    {
                        MapUnitData* sonRoom = room->next_room[i];
                        bool IsPathConnected = false;
                        path[i].clear();

                        for (int k = 0; k < sonRoom->entrance.size(); ++k)
                        {
                            Vec2 entrance = sonRoom->entrance[k];
                            Vec2 end = sonRoom->obstacle.low_left + entrance;
                            std::vector<Vec2> tmppath;

							// 进行A*寻路
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
                                    if (col.is_occupied) col.is_occupied = false;
                                }
                            }

							// 记录是否成功找到路径
                            if (result) IsPathConnected = true;
                        }

						// 若某子房间无法连接 则本次出口选择失败
                        if (!IsPathConnected)
                        {
                            IsExitCorrected = false;
                        }
                        else
                        {
							// 标记走廊路径复用
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

					// 若所有子房间均已连接 则更新当前最优解
                    if (IsExitCorrected)
                    {
                        if (!IsRoomConnected)
                        {
                            IsRoomConnected = true;
                            for (int i = 0; i < room->next_room.size(); ++i)
                            {
                                room->next_room[i]->path = path[i];
                                room->next_room[i]->chosen_entrance = entIndex[i];
                            }
                            room->chosen_exit = j;
                        }
                        else
                        {
                            int prelen = 0, nowlen = 0;
                            for (int i = 0; i < room->next_room.size(); ++i)
                            {
                                prelen += room->next_room[i]->path.size();
                                nowlen += path[i].size();
                            }

                            if (nowlen < prelen)
                            {
                                for (int i = 0; i < room->next_room.size(); ++i)
                                {
                                    room->next_room[i]->path = path[i];
                                    room->next_room[i]->chosen_entrance = entIndex[i];
                                }
                                room->chosen_exit = j;
                            }
                        }
                    }
                }

				// 若无法连接所有子房间 则本次走廊生成失败 重新生成房间位置
                if (!IsRoomConnected) {
                    IsRoomConnectedGlobal = false;
                    break;
                }
                else
                {

					// 在地图数组中标记走廊位置以避免后续走廊生成时冲突
                    for (MapUnitData* sonRoom : room->next_room)
                    {
                        std::vector<Vec2>& path = sonRoom->path;
                        int n = path.size();
                        for (int i = 0; i < n; ++i)
                        {
                            Vec2 pos = path[i];

                            if (i == 0 || i == n - 1) continue;

                            Vec2 dirToStart = path[i] - path[i - 1];
                            Vec2 dirToEnd = path[i + 1] - path[i];

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

                                        if (i >= n - 4)
                                        {
                                            if (dirToEnd.x > 0 && dx > 0) continue;
                                            if (dirToEnd.x < 0 && dx < 0) continue;
                                            if (dirToEnd.y > 0 && dy > 0) continue;
                                            if (dirToEnd.y < 0 && dy < 0) continue;
                                        }

                                        int tx = pos.x + dx, ty = pos.y + dy;
                                        MapArray[tx][ty] = BLOCK_WEIGHT;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (!IsRoomConnectedGlobal)  continue; 

            return;
        }
    }
}

// A*寻路算法实现
bool PrisonMapGen::FindPath(std::vector<std::vector<int>>& mapArray, std::vector<std::vector<AStarNode>>& AStarGraph, Vec2 startPos, Vec2 endPos, std::vector<Vec2>& path)
{

	// 定义优先队列 用于存储待探索节点
	compare cmp(AStarGraph);
	std::priority_queue<Vec2, std::vector<Vec2>, compare> frontier(cmp);
	AStarGraph[startPos.x][startPos.y].is_occupied = true;
	AStarGraph[startPos.x][startPos.y].g = 0;
	CalH(startPos, endPos, AStarGraph);
	AStarGraph[startPos.x][startPos.y].f = AStarGraph[startPos.x][startPos.y].g + AStarGraph[startPos.x][startPos.y].h;
	frontier.push(startPos);

	Vec2 dir[4] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };

	// 开始A*搜索
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

				path.push_back(startPos);

				int sz = path.size();
				for (int i = 0; i < sz / 2; ++i) std::swap(path[i], path[sz - 1 - i]);

				return true;

			}

			if (IsOut(pos, AStarGraph)) continue;

			if (AStarGraph[pos.x][pos.y].is_occupied || (mapArray[pos.x][pos.y] == BLOCK_WEIGHT)) continue;

			// 检查走廊宽度是否足够
			bool IsWidthEnough = true;
			for (int j = 0; j <= 1; ++j)
			{

				Vec2 point = pos;

				for (int k = 0; k <= 2; ++k)
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

			// 判断转角处宽度是否足够
			if (AStarGraph[front.x][front.y].last_dir != -1 && i != AStarGraph[front.x][front.y].last_dir)
			{

				for (int j = 1; j <= 3; ++j)
				{

					Vec2 point = pos + dir[AStarGraph[front.x][front.y].last_dir] * j;

					for (int k = 0; k <= 3; ++k)
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

			// 计算G、H、F值并将节点加入优先队列
			AStarGraph[pos.x][pos.y].g = AStarGraph[front.x][front.y].g + mapArray[pos.x][pos.y];
			if (i == 2 || i == 3)
			{

				if (AStarGraph[front.x][front.y].last_dir != i) AStarGraph[pos.x][pos.y].height = 0;
				else AStarGraph[pos.x][pos.y].height = AStarGraph[front.x][front.y].height + 1;
				if (!AStarGraph[pos.x][pos.y].extra) AStarGraph[pos.x][pos.y].g += 1 + AStarGraph[pos.x][pos.y].height;

			}
			else
			{

				AStarGraph[pos.x][pos.y].height = 0;
				// 共享路径不增加G值
				if (!AStarGraph[pos.x][pos.y].extra) AStarGraph[pos.x][pos.y].g += 1;

			}
			AStarGraph[pos.x][pos.y].last_dir = i;
			// 增加转弯惩罚
			if (AStarGraph[front.x][front.y].last_dir != -1 && AStarGraph[front.x][front.y].last_dir != AStarGraph[pos.x][pos.y].last_dir) AStarGraph[pos.x][pos.y].g += TURN_PUNISH;
			CalH(pos, endPos, AStarGraph);
			AStarGraph[pos.x][pos.y].f = AStarGraph[pos.x][pos.y].g + AStarGraph[pos.x][pos.y].h;
			AStarGraph[pos.x][pos.y].is_occupied = true;
			AStarGraph[pos.x][pos.y].last = front;

			frontier.push(pos);

		}

	}

	return false;

}