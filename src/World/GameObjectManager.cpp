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

#include "World\GameObjectManager.h"

#include <Ogre.h>

#include "Network\NetworkManager.h"
#include "Physics\PhysicsManager.h"
#include "World\PhysicsObject.h"
#include "Network\GameClient.h"
#include "World\GameObject.h"
#include "World\SpawnPoint.h"
#include "Physics\Vehicle.h"
#include "World\World.h"

#include "Event\EventManager.h"
#include "Player\PlayerManager.h"
#include "Player\Player.h"
#include "GODLoader.h"
#include "Utils.h"
#include "Game.h"
#include "NBID.h"

using namespace Ogre;

GameObjectManager::GameObjectManager(SceneManager* sm)
	: m_logger(NULL), m_physicsManager(NULL),
	m_sceneMgr(sm), m_godLoader(NULL)
{
	m_logger = Game::getSingleton().getLog();

	m_godLoader = &GODLoader::getSingleton();
	m_physicsManager = new PhysicsManager(this, sm);

	m_logger->logMessage("GameObjectManager initialized");
}

GameObjectManager::~GameObjectManager()
{
	for (Vehicle* vehicle : m_playerVehicles)
	{
		SAFE_DELETE(vehicle);
	}
	SAFE_DELETE(m_physicsManager);

	for (auto& go : m_gameObjects)
	{
		SAFE_DELETE(go);
	}
	m_gameObjects.clear();

	m_logger->logMessage("GameObjectManager destroyed");
}

void GameObjectManager::buildGameWorld(const std::vector<WorldObject*>& gameObjects)
{
	for (auto& worldObj : gameObjects)
	{
		m_logger->logMessage("Adding " + worldObj->getID());
		const int type = GameObject::getTypeByTag(worldObj->getTag());
		GameObject* obj = create(worldObj, type);
		if (obj->isEssential())
		{
			m_physicsManager->setUpPhysicsObject(static_cast<PhysicsObject*> (obj));
		}
	}

	m_logger->logMessage("Game world built");
}

void GameObjectManager::update(Real delta)
{
	m_physicsManager->update(delta);
	for (Vehicle* vehicle : m_playerVehicles)
	{
		vehicle->update(delta);
	}
}

GameObject* GameObjectManager::create(WorldObject* to, int type)
{
	GameObject* obj = NULL;
	switch (type)
	{
	case GameObjectType::TYPE_STATIC:
	case GameObjectType::TYPE_CHECKPOINT:
		obj = new PhysicsObject(this, to->getNode(), to->getEntity(), to->getObjectName());
		break;
	default:
		break;
	}

	obj->setType(type);
	m_gameObjects.push_back(obj);

	m_logger->logMessage(obj->getID() + " created");

	return obj;
}

GameObject* GameObjectManager::createManually(const String& tag, const String& id)
{
	SceneNode* node = m_sceneMgr->getRootSceneNode()->createChildSceneNode(id + "Node");
	node->setPosition(Vector3(0, 0, 0));
	node->setOrientation(Quaternion());
	node->setScale(Vector3(1, 1, 1));
	auto params = GODLoader::getSingleton().getGameObjectDefinitionByTag(id)->getParameters();
	const String& meshName = params["meshFile"];
	Entity* ent = m_sceneMgr->createEntity(id + "Entity", meshName);
	// TODO: Default shadow casting value ??
	ent->setCastShadows(true);
	node->attachObject(ent);

	const Ogre::String objName = tag + "_" + id;

	GameObject* go = NULL;
	go = new GameObject(this, node, ent, objName);
	m_gameObjects.push_back(go);

	return go;
}

Ogre::ManualObject* GameObjectManager::create2DManualObject(const Ogre::String& matName, Ogre::SceneManager* sm,
	Ogre::Real scale, bool isDynamic, bool clear, Ogre::Real mul, const Ogre::Vector2& offset,
	Ogre::uint32 vis, Ogre::uint8 rendeq, bool comb)
{
	Ogre::ManualObject* mo = sm->createManualObject();
	mo->setDynamic(isDynamic);
	mo->setUseIdentityProjection(true);
	mo->setUseIdentityView(true);
	mo->setCastShadows(false);

	mo->estimateVertexCount(comb ? 8 : 4);
	mo->begin(matName, comb ? Ogre::RenderOperation::OT_TRIANGLE_LIST : Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	static const Vector2 uv[4] = { Vector2(0.f, 1.f), Vector2(1.f, 1.f), Vector2(0.f, 0.f), Vector2(1.f, 0.f) };
	int n = comb ? 2 : 1;
	const float asp = (Game::getSingleton().getWindow()->getWidth() / Game::getSingleton().getWindow()->getHeight());
	for (int i = 0; i < n; ++i)
	{
		mo->position(-scale, -scale*asp, 0);
		mo->textureCoord(uv[0] * mul + offset);  if (clear)  mo->colour(0, 1, 0);

		mo->position(scale, -scale*asp, 0);
		mo->textureCoord(uv[1] * mul + offset);  if (clear)  mo->colour(0, 0, 0);

		mo->position(-scale, scale*asp, 0);
		mo->textureCoord(uv[2] * mul + offset);  if (clear)  mo->colour(1, 1, 0);

		mo->position(scale, scale*asp, 0);
		mo->textureCoord(uv[3] * mul + offset);  if (clear)  mo->colour(1, 0, 0);
	}
	if (comb)
	{
		mo->quad(0, 1, 3, 2);
		mo->quad(4, 5, 7, 6);
	}
	mo->end();

	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();

	mo->setBoundingBox(aabInf);
	mo->setVisibilityFlags(vis);
	mo->setRenderQueueGroup(rendeq);

	return mo;
}

Vehicle* GameObjectManager::createVehicle(const Ogre::String& vehicleName, NBID* id)
{
	SpawnPoint* spawnData = NULL;
	if (Game::getGameMode() == GameMode::PRACTICE)
	{
		spawnData = World::getSpawnPoint("pole");
	}
	else
	{
		GameClient* client = NetworkManager::getSingleton().getGameClientByID(id);
		spawnData = World::getSpawnPoint("pole", client->getPhotonID() - 1);
	}

	assert(spawnData != NULL && "Spawn data name does not match!");
	const std::string playerID = id->toString();
	SceneNode* vehicleNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode(playerID + "Node");
	vehicleNode->setPosition(spawnData->getPosition());
	vehicleNode->setOrientation(spawnData->getRotation());
	vehicleNode->setScale(spawnData->getScale());
	auto params = GODLoader::getSingleton().getGameObjectDefinitionByTag(vehicleName)->getParameters();
	Entity* vehicleEntity = m_sceneMgr->createEntity(playerID + "Entity", params["meshFile"]);
	vehicleEntity->setCastShadows(true);
	vehicleNode->attachObject(vehicleEntity);
	const Ogre::String objectName = vehicleName + "_" + playerID;

	Vehicle* vehicle = new Vehicle(this, vehicleNode, vehicleEntity, objectName);
	vehicle->setType(GameObjectType::TYPE_COMBAT_VEHICLE);
	m_physicsManager->setUpPhysicsObject(vehicle);
	vehicle->setOwnerPlayer(PlayerManager::getSingleton().getPlayerByID(id));
	vehicle->setState(VehicleState::INIT_COMPLETE); // Update vehicle's state when init completed

	m_playerVehicles.push_back(vehicle);

	EventManager::getSingleton().notify(EventType::PLAYER_VEHICLE_READY, id);

	return vehicle;
}

GameObject* GameObjectManager::getGameObjectByID(const Ogre::String& name)
{
	for (const auto& go : m_gameObjects)
	{
		if (go->getID() == name)
		{
			return go;
		}
	}

	return NULL;
}

Vehicle* GameObjectManager::getVehicleByID(NBID* id) const
{
	for (Vehicle* vehicle : m_playerVehicles)
	{
		if (vehicle->getOwnerPlayer()->getID() == id)
		{
			return vehicle;
		}
	}

	return NULL;
}

void GameObjectManager::destroy(GameObject* obj)
{
	m_logger->logMessage("Destroying GameObject " + obj->getID());
	if (static_cast<PhysicsObject*> (obj))
	{
		m_physicsManager->destroy(static_cast<PhysicsObject*> (obj));
	}
	m_gameObjects.erase(std::remove(m_gameObjects.begin(), m_gameObjects.end(), obj));
	SAFE_DELETE(obj);
}

void GameObjectManager::destroyVehicle(Vehicle* vehicle)
{
	m_playerVehicles.erase(std::remove(m_playerVehicles.begin(), m_playerVehicles.end(), vehicle));
	m_physicsManager->destroyVehicle(vehicle);
	SAFE_DELETE(vehicle);
}