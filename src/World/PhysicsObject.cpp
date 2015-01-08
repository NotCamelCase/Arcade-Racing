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