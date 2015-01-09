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

#include "World\PhysicsObject.h"

#include <Ogre.h>

#include "Physics\BtOgrePG.h"
#include "Physics\BtOgreGP.h"

#include "World\GameObjectManager.h"
#include "Event\CollisionListener.h"
#include "Utils.h"

PhysicsObject::PhysicsObject(GameObjectManager* mng, Ogre::SceneNode* node, Ogre::Entity* entity, const Ogre::String& id)
	: GameObject(mng, node, entity, id), m_dynamic(false),
	m_collisionShape(NULL), m_rigidBody(NULL), m_motionState(NULL)
{
}

PhysicsObject::~PhysicsObject()
{
	SAFE_DELETE(m_collisionShape);
	SAFE_DELETE(m_motionState);
}

PhysicsObject* PhysicsObject::getOtherOffCollision(CollisionEventData* data)
{
	if (this == data->collider)
		return data->collidingWith;
	else
		return data->collider;
}

void PhysicsObject::update(Ogre::Real delta)
{
}