#pragma once

#include <OgreString.h>

namespace Ogre { class SceneNode; class Entity; }

class WorldObject
{
public:
	WorldObject(Ogre::SceneNode* node, Ogre::Entity* ent, const Ogre::String& name);
	virtual ~WorldObject();

	Ogre::SceneNode* getNode() const { return m_node; }
	Ogre::Entity* getEntity() const { return m_entity; }

	Ogre::String getID() const { return m_id; }
	Ogre::String getTag() const { return m_tag; }
	Ogre::String getObjectName() const { return m_objectName; }

	// Essential game objects have physics applied upon them
	bool isEssential() const { return m_essential; }

	// Helper objects have no physics components
	bool isHelper() const { return !isEssential(); }

protected:
	Ogre::SceneNode* m_node;
	Ogre::Entity* m_entity;

	Ogre::String m_objectName;
	Ogre::String m_id;
	Ogre::String m_tag;

	bool m_essential;

private:
	WorldObject(const WorldObject&);
};