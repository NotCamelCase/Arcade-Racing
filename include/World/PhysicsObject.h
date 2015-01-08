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