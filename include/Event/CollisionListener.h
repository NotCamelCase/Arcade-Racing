#pragma once

#include "IEventListener.h"

class PhysicsObject;
class btManifoldPoint;

struct CollisionEventData
{
	CollisionEventData(const btManifoldPoint& cp)
		: contactPoint(cp)
	{
	}

	~CollisionEventData()
	{
	}

	PhysicsObject* collider;
	PhysicsObject* collidingWith;
	
	const btManifoldPoint& contactPoint;

	bool operator==(CollisionEventData* other);
	bool operator!=(CollisionEventData* other);
};

class CollisionListener : public IEventListener
{
public:
	CollisionListener();
	~CollisionListener();

	virtual void onCollisionStart(CollisionEventData* data) = 0;
	virtual void onCollisionTime(CollisionEventData* data) = 0;
	virtual void onCollisionEnd(CollisionEventData* data) = 0;

	virtual void onEvent(int eventType, void* data = 0);
};