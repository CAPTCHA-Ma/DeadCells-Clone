#include "FlyingObject.h"
#include "Arrow.h"
#include "Bomb.h"
USING_NS_CC;
FlyingObject* FlyingObject::create(FlyType type, bool fromPlayer,float atkPower)
{
	switch (type)
	{
		case FlyType::Arrow:
			return Arrow::create(fromPlayer, atkPower);
			break;
		case FlyType::Bomb:
			return Bomb::create(atkPower);
			break;
		default:
			break;
	}
}