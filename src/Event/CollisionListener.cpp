#include "Event\CollisionListener.h"

#include "World\PhysicsObject.h"

CollisionListener::CollisionListener()
{
}

CollisionListener::~CollisionListener()
{
}

void CollisionListener::onEvent(int eventType, void* data)
{
	switch (eventType)
	{
	case EventType::COLLISION_STARTED:
		onCollisionStart(static_cast<CollisionEventData*> (data));
		break;
	case EventType::COLLISION_TIME:
		onCollisionTime(static_cast<CollisionEventData*> (data));
		break;
	case EventType::COLLISION_ENDED:
		onCollisionEnd(static_cast<CollisionEventData*> (data));
		break;
	}
}

bool CollisionEventData::operator==(CollisionEventData* other)
{
	return ((collider == other->collider && collidingWith == other->collidingWith)
		|| (collider == other->collidingWith && collidingWith == other->collider));
}

bool CollisionEventData::operator!=(CollisionEventData* other)
{
	return !((collider == other->collider && collidingWith == other->collidingWith)
		|| (collider == other->collidingWith && collidingWith == other->collider));
}