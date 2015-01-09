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

#include "World\GameObject.h"

#include <OgrePrerequisites.h>

namespace Ogre
{
	class Entity;
	class SceneNode;
}

namespace BtOgre
{
	class RigidBodyState;
}

class btRigidBody;
class btCollisionShape;
class GameObjectManager;
struct CollisionEventData;

class PhysicsObject : public GameObject
{
public:
	PhysicsObject(GameObjectManager* mng, Ogre::SceneNode* node, Ogre::Entity* entity, const Ogre::String& id);
	~PhysicsObject();

	btCollisionShape* getCollisionShape() const { return m_collisionShape; }
	btRigidBody* getRigidBody() const { return m_rigidBody; }
	BtOgre::RigidBodyState* getMotionState() const { return m_motionState; }

	void setCollisionShape(btCollisionShape* shape) { m_collisionShape = shape; }
	void setRigidBody(btRigidBody* body) { m_rigidBody = body; }
	void setMotionState(BtOgre::RigidBodyState* state) { m_motionState = state; }

	virtual void update(Ogre::Real delta);

	bool isStatic() const { return !m_dynamic; }
	bool isDynamic() const { return m_dynamic; }

	void setDynamic(bool v) { m_dynamic = v; }
	void setStatic(bool v) { m_dynamic = !v; }

	PhysicsObject* getOtherOffCollision(CollisionEventData* data);
	
protected:
	bool m_dynamic;

	btCollisionShape* m_collisionShape;
	btRigidBody* m_rigidBody;
	BtOgre::RigidBodyState* m_motionState;
};