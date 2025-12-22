#include "FlyingObject.h"
#include "Arrow.h"
#include "Bomb.h"
USING_NS_CC;
FlyingObject* FlyingObject::create(FlyType type, bool fromPlayer)
{
	switch (type)
	{
		case FlyType::Arrow:
			return Arrow::create(fromPlayer);
			break;
		case FlyType::Bomb:
			return Bomb::create();
			break;
		default:
			break;
	}
}