#include "Prison.h"
#include "Res/strings.h"
#include <queue>
USING_NS_CC;

bool SearchForPos(Prison* scene, int k)
{

	MapUnitData* corridorRoom = scene->PrisonMapData->SceneMapDataUnit[k];
	int sonNum = int(corridorRoom->nextRoom.size());

	door exit = corridorRoom->preRoom->exit[0];



	return true;

}

bool Prison::InitPrisonData()
{

	//Sleep(2000);

	PrisonMapData = new SceneMapData;

	int AllNum = RandomHelper::random_int(11, 14),
		eliteNum = RandomHelper::random_int(1, 2),
		treasureNum = RandomHelper::random_int(1, 2),
		traderNum = RandomHelper::random_int(0, 1),
		exitNum = 1;
	int specialNum = eliteNum + treasureNum + traderNum + exitNum,
		tripleNum = RandomHelper::random_int(1, (specialNum - 1) / 2),
		doubleNum = specialNum - tripleNum * 2 - 1,
		singleNum = AllNum - specialNum - doubleNum - tripleNum;
	int sonNum[4] = { specialNum, singleNum, doubleNum, tripleNum }, specialRoomNum[4] = { eliteNum, treasureNum, traderNum, exitNum };

	CCLOG("%d %d %d %d", sonNum[0], sonNum[1], sonNum[2], sonNum[3]);

	std::queue<MapUnitData*> roomQueue;
	MapUnitData* startRoomData = new MapUnitData;
	startRoomData->roomtype = Type::start;
	startRoomData->name = "PrisonStart.tmx";
	startRoomData->preRoom = nullptr;
	PrisonMapData->SceneMapDataUnit.push_back(startRoomData);
	roomQueue.push(startRoomData);

	int counter = 0;

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

			for (int i = 0; i < 4; ++i)
			{

				if (specialRoomNum[i]) specialRoomPool.insert(specialRoomPool.end(), specialRoomNum[i], i);

			}

			int chosenSpecialRoomNum = specialRoomPool[RandomHelper::random_int(0, int(specialRoomPool.size() - 1))];
			--specialRoomNum[chosenSpecialRoomNum];
			room->roomtype = static_cast<Type>(chosenSpecialRoomNum);
			
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

			if (room != startRoomData) room->roomtype = Type::combat;

			MapUnitData* corridorRoomData = new MapUnitData;
			corridorRoomData->roomtype = Type::corridor;
			room->nextRoom.push_back(corridorRoomData);
			corridorRoomData->preRoom = room;
			PrisonMapData->SceneMapDataUnit.push_back(corridorRoomData);

			while (chosenSonNum--)
			{

				MapUnitData* sonRoomData = new MapUnitData;
				sonRoomData->preRoom = corridorRoomData;
				corridorRoomData->nextRoom.push_back(sonRoomData);
				roomQueue.push(sonRoomData);
				PrisonMapData->SceneMapDataUnit.push_back(sonRoomData);

			}

		}

	}

	MapDataManager* mDM = MapDataManager::getInstance();
	startRoomData->obstacle.push_back(Box({ Vec2::ZERO, Vec2(static_cast<float>(mDM->getRoomData("PrisonStart.tmx")->width), static_cast<float>(mDM->getRoomData("PrisonStart.tmx")->width)) }));
	startRoomData->exit.push_back(door({ Direction::right, mDM->getRoomData("PrisonStart.tmx")->exits.right[0]}));

	SearchForPos(this, 1);

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