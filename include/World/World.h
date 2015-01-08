#pragma once

#include <vector>
#include <map>

#include <rapidxml.hpp>

#include <OgreString.h>

class SpawnPoint;
class WorldObject;

namespace Ogre
{
	class Log; class SceneNode; class Entity; class SceneManager;
	class ColourValue; class Vector3; class Quaternion; class StaticGeometry;
}

class World
{
public:
	World(Ogre::SceneManager* sm, const Ogre::String& worldName);
	~World();

	void load();
	void dispose();

	bool isLoaded() const { return m_loaded; }
	void setLoaded(bool v) { m_loaded = v; }

	static SpawnPoint* getSpawnPoint(const Ogre::String& name);
	static SpawnPoint* getSpawnPoint(const Ogre::String& name, int index);
	static std::vector<SpawnPoint*>& getSpawnPoints(const Ogre::String& name);

	Ogre::String getWorldID() const { return m_worldName; }
	std::vector<WorldObject*>& getWorldObjects() { return m_worldObjects; }

	void setValidated(bool val);
	bool isValidated() const { return m_validated; }

private:
	Ogre::String m_worldName;
	std::vector<WorldObject*> m_worldObjects;
	std::map<Ogre::String, Ogre::StaticGeometry*> m_staticGeometries;
	static std::map<Ogre::String, std::vector<SpawnPoint*>> m_spawnData;

	Ogre::Log* m_logger;
	Ogre::SceneManager* m_sceneMgr;

	bool m_loaded;
	bool m_validated;

	void scanNodes(rapidxml::xml_node<>* nodes);
	void scanNode(rapidxml::xml_node<>* node);
	WorldObject* scanEntity(rapidxml::xml_node<>* parentNode, rapidxml::xml_node<>* node);
	void scanSpawnData(rapidxml::xml_node<>* docNode);
	void scanCamera(rapidxml::xml_node<>* parentNode, rapidxml::xml_node<>* camNode);
	void scanLight(rapidxml::xml_node<>* parentNode, rapidxml::xml_node<>* lightNode);
	void scanEnvironment(rapidxml::xml_node<>* envNode);
	void scanStaticGeometries(rapidxml::xml_node<>* staticNode);

	SpawnPoint* createSpawnPoint(const Ogre::String& type, const Ogre::String& spawnID, const Ogre::Vector3& pos,
		const Ogre::Quaternion& rot, const Ogre::Vector3& scale);

	char* checkAttributeWithCaution(rapidxml::xml_node<>* checkNode, const Ogre::String& name);

	Ogre::SceneNode* createSceneNode(rapidxml::xml_node<>* docNode);

	Ogre::ColourValue parseColourValue(rapidxml::xml_node<>* colourNode);
	Ogre::Vector3 parsePosition(rapidxml::xml_node<>* posNode);
	Ogre::Quaternion parseQuaternion(rapidxml::xml_node<>* rotNode);
};