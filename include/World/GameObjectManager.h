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