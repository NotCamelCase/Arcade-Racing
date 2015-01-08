#pragma once

#include <vector>

#include <OgreString.h>

class NBID;
class GODLoader;
class GameObject;
class WorldObject;
class Vehicle;
class PhysicsManager;
class btManifoldPoint;

namespace Ogre { class Log; }

class GameObjectManager
{
public:
	GameObjectManager(Ogre::SceneManager* sm);
	~GameObjectManager();

	void update(Ogre::Real delta);
	void destroy(GameObject* po);
	void buildGameWorld(const std::vector<WorldObject*>& gameObjects);

	GameObject* create(WorldObject* go, int type);

	Ogre::ManualObject* create2DManualObject(const Ogre::String& matName, Ogre::SceneManager* sm,
		Ogre::Real scale, bool isDynamic, bool clear, Ogre::Real mul, const Ogre::Vector2& offset,
		Ogre::uint32 vis, Ogre::uint8 rendeq, bool comd);

	PhysicsManager* getPhysicsManager() const { return m_physicsManager; }

	Ogre::SceneManager* getSceneManager() const { return m_sceneMgr; }

	std::vector<GameObject*>& getGameObjects() { return m_gameObjects; }

	GameObject* createManually(const Ogre::String& tag, const Ogre::String& id);
	Vehicle* createVehicle(const Ogre::String& id, NBID* uuid);

	GameObject* getGameObjectByID(const Ogre::String& name);

	Vehicle* getVehicleByID(NBID* id) const;
	void destroyVehicle(Vehicle* vehicle);

	Ogre::Log* getLog() const { return m_logger; }

private:
	Ogre::SceneManager* m_sceneMgr;
	Ogre::Log* m_logger;

	std::vector<Vehicle*> m_playerVehicles;
	PhysicsManager* m_physicsManager;
	GODLoader* m_godLoader;

	std::vector<GameObject*> m_gameObjects;
};