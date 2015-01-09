/*
The MIT License (MIT)

Copyright (c) 2015 Tayfun Kayhan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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