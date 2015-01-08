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