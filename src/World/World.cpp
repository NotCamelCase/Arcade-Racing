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

#include "World\World.h"

#include <Ogre.h>

#include "World\WorldObject.h"
#include "World\SpawnPoint.h"

#include "Game.h"
#include "Utils.h"
#include "GODLoader.h"
#include "Player\RaceManager.h"

using namespace rapidxml;

#define MAX_BATCH_RENDERING_DISTANCE 10000

#define USE_FUS 1

using namespace Ogre;

std::map<Ogre::String, std::vector<SpawnPoint*>> World::m_spawnData;

World::World(SceneManager* sm, const String& worldName)
	: m_sceneMgr(sm), m_worldName(worldName), m_logger(NULL), m_loaded(false), m_validated(false)
{
	m_logger = Game::getSingleton().getLog();
	m_logger->logMessage("World being parsed");
}

World::~World()
{
	dispose();
}

void World::load()
{
	m_logger->logMessage("Loading " + m_worldName);

	m_worldName = GODLoader::getSingleton().getGameObjectDefinitionValue(RaceManager::getSingleton().getTrackData()->objName, "file_name");

	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(m_worldName, "General");
	char* scene = strdup(stream->getAsString().c_str());
	xml_document<>* doc = new xml_document<>();
	doc->parse<0>(scene);

	xml_node<>* mainSceneNode = doc->first_node("scene");
	scanEnvironment(mainSceneNode->first_node("environment"));
	scanStaticGeometries(mainSceneNode->first_node("staticGeometries"));
	scanNodes(mainSceneNode->first_node("nodes"));

	SAFE_DELETE(doc);
	SAFE_DELETE(scene);
	m_logger->logMessage("Scene structure parsed");

	m_loaded = true;
	m_logger->logMessage("World " + m_worldName + " has been loaded");
}

void World::scanNodes(xml_node<>* nodes)
{
	for (xml_node<>* headNode = nodes->first_node("node"); headNode; headNode = headNode->next_sibling("node"))
	{
		const String firstName = headNode->first_attribute("name")->value();
		scanNode(headNode);
		xml_node<>* subEntNode = headNode->first_node("node");
		for (; subEntNode; subEntNode = subEntNode->next_sibling())
		{
			scanNode(subEntNode);
		}
	}
}

void World::scanEnvironment(xml_node<>* envNode)
{
	m_logger->logMessage("Processing environment elements");
	xml_node<>* subNode = envNode->first_node("colourAmbient");

	m_sceneMgr->setAmbientLight(parseColourValue(envNode->first_node("colourAmbient")));

	// TODO: Set shadow technique
}

void World::scanStaticGeometries(xml_node<>* staticNode)
{
	xml_node<>* subStaticNode = staticNode->first_node("staticGeometry");
	for (; subStaticNode; subStaticNode = subStaticNode->next_sibling())
	{
		const String groupName = subStaticNode->first_attribute("name")->value();
		StaticGeometry* staticObject = NULL;
		auto it = m_staticGeometries.find(groupName);
		if (it == m_staticGeometries.end())
		{
			staticObject = m_sceneMgr->createStaticGeometry("StaticGeometry" + groupName);
			assert(staticObject != NULL);
			m_staticGeometries.insert(std::make_pair(groupName, staticObject));
			m_logger->logMessage("Static geometry " + groupName + " created");

			const bool castShadows = StringConverter::parseBool(subStaticNode->first_attribute("castShadows")->value());
			const bool visibility = StringConverter::parseBool(subStaticNode->first_attribute("visible")->value());
			Vector3& origin = parsePosition(subStaticNode->first_node("origin"));
			Vector3& dimensions = parsePosition(subStaticNode->first_node("dimensions"));

			staticObject->setOrigin(origin);
			staticObject->setRegionDimensions(dimensions);
			staticObject->setRenderingDistance(MAX_BATCH_RENDERING_DISTANCE);
			staticObject->setCastShadows(castShadows);
		}
		else
		{
			staticObject = it->second;
		}

		xml_node<>* entityParentNode = subStaticNode->first_node("entities");
		xml_node<>* entityNode = entityParentNode->first_node("entity");
		for (; entityNode; entityNode = entityNode->next_sibling())
		{
			WorldObject* sceneObject = scanEntity(entityNode, entityNode);
			assert(sceneObject != NULL && "Couldn't create WorldObject off Static Geometry!");
			SceneNode* sceneObjectNode = sceneObject->getNode();
			staticObject->addSceneNode(sceneObjectNode);
			sceneObjectNode->getParentSceneNode()->removeChild(sceneObjectNode);
		}
	}

	for (const auto& val : m_staticGeometries)
	{
		StaticGeometry* readyStatic = val.second;
		readyStatic->build();
		m_logger->logMessage("Static geometry " + readyStatic->getName() + " built");
	}
}

void World::scanNode(xml_node<>* node)
{
	xml_node<>* element = node->first_node("entity");
	if (element)
	{
		scanEntity(node, element);
	}
	else if ((element = node->first_node("camera")))
	{
		scanCamera(node, element);
	}
	else if ((element = node->first_node("light")))
	{
		scanLight(node, element);
	}
	else
	{
		scanSpawnData(node);
	}
}

// TODO: Remove nodeName arg once entities are renamed correctly in Blender
WorldObject* World::scanEntity(xml_node<>* parentNode, xml_node<>* entityNode)
{
	const String meshName = entityNode->first_attribute("meshFile")->value();
	String nodeName;
	const char* nameAttribRes = checkAttributeWithCaution(entityNode, "name");
	if (nameAttribRes == NULL)
	{
		// StaticGeometry, InstancedEntity, Dummy nodes in Maya have no "name" attribute exported
		nameAttribRes = checkAttributeWithCaution(parentNode, "name");
		if (nameAttribRes == NULL)
		{
			nodeName = meshName + StringConverter::toString(Math::RangeRandom(0, 1000));
		}
	}
	else {
		nodeName = nameAttribRes;
	}

	Entity* entity = m_sceneMgr->createEntity(nodeName, meshName);
	SceneNode* parent = createSceneNode(parentNode);
	parent->attachObject(entity);

	const char* castCheckRes = checkAttributeWithCaution(entityNode, "castShadows");
	if (castCheckRes != NULL)
	{
		bool isCastingShadows = StringConverter::parseBool(castCheckRes);
		entity->setCastShadows(isCastingShadows);
	}

	xml_node<>* subEntityXMLNode = entityNode->first_node("subentities");
	if (subEntityXMLNode)
	{
		xml_node<>* subEntityNode = entityNode->first_node("subentities")->first_node();
		if (subEntityNode)
		{
			bool isReceivingShadows = true;
			castCheckRes = checkAttributeWithCaution(entityNode, "receiveShadows");
			if (castCheckRes != NULL)
			{
				isReceivingShadows = StringConverter::parseBool(castCheckRes);
			}

			for (; subEntityNode; subEntityNode = subEntityNode->next_sibling())
			{
				const String materialName = subEntityNode->first_attribute("materialName")->value();
				const size_t matIndex = StringConverter::parseSizeT(subEntityNode->first_attribute("index")->value());
				MaterialPtr entityMat = MaterialManager::getSingletonPtr()->getByName(materialName);
				entityMat->setReceiveShadows(isReceivingShadows);
				entity->getSubEntity(matIndex)->setMaterialName(materialName);
			}
		}
	}

	WorldObject* obj = new WorldObject(parent, entity, nodeName);
	m_logger->logMessage("World object " + nodeName + " created");
	m_worldObjects.push_back(obj);

	return obj;
}

void World::scanSpawnData(xml_node<>* docNode)
{
	const Vector3& pos = parsePosition(docNode->first_node("position"));
	const Vector3& scale = parsePosition(docNode->first_node("scale"));
	const Quaternion& rot = parseQuaternion(docNode->first_node("rotation"));
	const String nodeName = docNode->first_attribute("name")->value();
	const StringVector nameParts = StringUtil::split(nodeName, "_");
	const String& type = nameParts[1];
	const String& id = nameParts[2];

	createSpawnPoint(type, id, pos, rot, scale);
}

void World::scanCamera(xml_node<>* parentNode, xml_node<>* camNode)
{
	const String name = camNode->first_attribute("name")->value();
	Camera* camera = m_sceneMgr->getCamera("Camera1");

	Vector3& pos = parsePosition(parentNode->first_node("position"));
	Quaternion& rot = parseQuaternion(parentNode->first_node("rotation"));

	camera->setPosition(pos);
	camera->setOrientation(rot);
	camera->lookAt(0, 0, 0);
	camera->setFOVy(Radian(StringConverter::parseReal(camNode->first_attribute("fov")->value())));

	xml_node<>* child = camNode->first_node("clipping");
	camera->setNearClipDistance(StringConverter::parseReal(child->first_attribute("near")->value()));
	camera->setFarClipDistance(StringConverter::parseReal(child->first_attribute("far")->value()));
}

void World::scanLight(xml_node<>* parentNode, xml_node<>* lightNode)
{
	const String name = lightNode->first_attribute("name")->value();
	Light* light = m_sceneMgr->createLight(name);

	const String type = lightNode->first_attribute("type")->value();
	if (type == "directional")
		light->setType(Light::LightTypes::LT_DIRECTIONAL);
	else if (type == "spot")
		light->setType(Light::LightTypes::LT_SPOTLIGHT);
	else if (type == "point")
		light->setType(Light::LightTypes::LT_POINT);

	light->setPowerScale(StringConverter::parseReal(lightNode->first_attribute("power")->value())); //powerScale
	const bool castShadow = true; // FIXME: StringConverter::parseBool(lightNode->first_attribute("castShadows")->value());
	light->setCastShadows(castShadow);

	xml_node<>* atten = lightNode->first_node("lightAttenuation");
	if (atten)
	{
		const Real linear = StringConverter::parseReal(atten->first_attribute("linear")->value());
		const Real range = StringConverter::parseReal(atten->first_attribute("range")->value());
		const Real constant = StringConverter::parseReal(atten->first_attribute("constant")->value());
		const Real quad = StringConverter::parseReal(atten->first_attribute("quadric")->value()); // quadratic
		light->setAttenuation(range, constant, linear, quad);
	}
	
	light->setPosition(parsePosition(parentNode->first_node("position")));
	//light->setDirection((parseQuaternion(lightNode->first_node("rotation")) * light->getPosition()));
	light->setDirection(Vector3(0, -1, 1));

	light->setDiffuseColour(parseColourValue(lightNode->first_node("colourDiffuse")));
	if (lightNode->first_node("colourSpecular"))
		light->setSpecularColour(parseColourValue(lightNode->first_node("colourSpecular")));
	
	const char* attRes = checkAttributeWithCaution(lightNode, "colourShadow");
	if (attRes != NULL)
	{
		m_sceneMgr->setShadowColour(StringConverter::parseColourValue(attRes));
		attRes = NULL;
	}

	/*SceneNode* parent = m_sceneMgr->getRootSceneNode()->createChildSceneNode(name + "Parent");
	parent->setPosition(parsePosition(parentNode->first_node("position")));
	parent->setOrientation(parseQuaternion(parentNode->first_node("rotation")));
	parent->attachObject(light);*/
}

Quaternion World::parseQuaternion(xml_node<>* rotNode)
{
	Quaternion rot;
	rot.x = StringConverter::parseReal(rotNode->first_attribute("qx")->value());
	rot.y = StringConverter::parseReal(rotNode->first_attribute("qy")->value());
	rot.z = StringConverter::parseReal(rotNode->first_attribute("qz")->value());
	rot.w = StringConverter::parseReal(rotNode->first_attribute("qw")->value());

	return rot;
}

Vector3 World::parsePosition(xml_node<>* posNode)
{
	Vector3 pos;
	pos.x = StringConverter::parseReal(posNode->first_attribute("x")->value());
	pos.y = StringConverter::parseReal(posNode->first_attribute("y")->value());
	pos.z = StringConverter::parseReal(posNode->first_attribute("z")->value());

	return pos;
}

ColourValue World::parseColourValue(xml_node<>* colourNode)
{
	ColourValue col;
	col.r = StringConverter::parseReal(colourNode->first_attribute("r")->value());
	col.g = StringConverter::parseReal(colourNode->first_attribute("g")->value());
	col.b = StringConverter::parseReal(colourNode->first_attribute("b")->value());

	return col;
}

SceneNode* World::createSceneNode(xml_node<>* docNode)
{
	Vector3& pos = parsePosition(docNode->first_node("position"));
	Quaternion& rot = parseQuaternion(docNode->first_node("rotation"));
#if USE_FUS
	const Vector3& scale = Vector3::UNIT_SCALE;
#else
	Vector3& scale = parsePosition(docNode->first_node("scale"));
#endif
	
	String name;
	const char* checkRes = checkAttributeWithCaution(docNode, "name");
	if (checkRes != NULL)
		name = docNode->first_attribute("name")->value();
	else {
		name = docNode->first_attribute("meshFile")->value()
			+ StringConverter::toString(Math::RangeRandom(0, 1000));
	}

	SceneNode* sceneNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	assert(sceneNode != NULL);
	sceneNode->setPosition(pos);
	sceneNode->setOrientation(rot);
	sceneNode->setScale(scale);

	return sceneNode;
}

char* World::checkAttributeWithCaution(rapidxml::xml_node<>* checkNode, const String& name)
{
	char* retVal = NULL;
	xml_attribute<>* attrib = checkNode->first_attribute(name.c_str());
	if (attrib)
	{
		retVal = attrib->value();
	}

	return retVal;
}

void World::dispose()
{
	for (auto& val : m_staticGeometries)
	{
		StaticGeometry* sg = val.second;
		if (sg)
		{
			sg->destroy();
		}
	}
	m_staticGeometries.clear();
	m_logger->logMessage("Static geometries destroyed");

	for (auto& elem : m_spawnData)
	{
		for (auto* data : elem.second)
		{
			SAFE_DELETE(data);
		}
		elem.second.clear();
	}
	m_spawnData.clear();
	m_logger->logMessage("SpawnPoints destroyed");

	// FIXME : Destroy entities, nodes in better order
	m_sceneMgr->clearScene();

	m_logger->logMessage(m_worldName + " destroyed");
}

void World::setValidated(bool v)
{
	m_validated = v;
	if (m_validated)
	{
		if (isLoaded())
		{
			if (!m_worldObjects.empty())
				m_worldObjects.clear();
		}
	}
}

SpawnPoint* World::getSpawnPoint(const Ogre::String& type)
{
	SpawnPoint* spawn = NULL;
	spawn = getSpawnPoints(type).front();

	return spawn;
}

SpawnPoint* World::getSpawnPoint(const Ogre::String& type, int index)
{
	SpawnPoint* spawn = NULL;
	spawn = getSpawnPoints(type)[index];

	return spawn;
}

std::vector<SpawnPoint*>& World::getSpawnPoints(const Ogre::String& name)
{
	return (m_spawnData.find(name)->second);
}

SpawnPoint* World::createSpawnPoint(const Ogre::String& type, const Ogre::String& spawnID, const Ogre::Vector3& pos,
	const Ogre::Quaternion& rot, const Ogre::Vector3& scale)
{
	SpawnPoint* spawn = NULL;
	spawn = new SpawnPoint(spawnID, pos, rot, scale);
	std::vector<SpawnPoint*>& spawnList = m_spawnData[type];
	spawnList.push_back(spawn);

	return spawn;
}