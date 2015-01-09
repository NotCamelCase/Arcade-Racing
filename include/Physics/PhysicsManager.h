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

#include <vector>
#include <set>

#include <OgrePrerequisites.h>

class Vehicle;
class GODLoader;
class PhysicsObject;
class GameObjectManager;
struct CollisionEventData;
class btDynamicsWorld;
class btCollisionShape;
class btManifoldPoint;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btSequentialImpulseConstraintSolver;

namespace Ogre { class SceneManager; class Log; }
namespace BtOgre { class DebugDrawer; }

class PhysicsManager
{
public:
	PhysicsManager(GameObjectManager* gom, Ogre::SceneManager* sceneMgr);
	~PhysicsManager();

	void update(Ogre::Real delta);
	void checkCollisions(Ogre::Real timeStep);

	bool isDebugging() const { return m_debugging; }
	void setDebugging(bool v) { m_debugging = v; }

	void setUpPhysicsObject(PhysicsObject* obj);
	void destroyVehicle(Vehicle* vehicle);
	void destroy(PhysicsObject* po);

private:
	Ogre::SceneManager* m_sceneMgr;
	Ogre::Log* m_logger;

	GODLoader* m_godLoader;
	GameObjectManager* m_gameObjManager;

	btDynamicsWorld* m_btWorld;
	BtOgre::DebugDrawer* m_debugDrawer;

	btBroadphaseInterface* m_broadphase;
	btDefaultCollisionConfiguration* m_collisionConfig;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;

	enum CollisionType
	{
		STARTED,
		COLLIDING,
		ENDED
	};

	struct CollisionInfo
	{
		CollisionType type;
		btManifoldPoint& contactPoint;

		CollisionInfo(btManifoldPoint& point) : type(CollisionType::STARTED), contactPoint(point) {}
	};

	std::map<std::pair<PhysicsObject*, PhysicsObject*>, CollisionInfo> m_collidingObjects;

	void setUpMainProperties(PhysicsObject* object);
	void setUpVehicle(Vehicle* vehicle);

	btCollisionShape* createCollisionShape(Ogre::Entity* ent, const Ogre::String& type, bool animated = false);

	bool m_debugging;
};