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