#include "World\WorldObject.h"

#include <OgreStringConverter.h>

WorldObject::WorldObject(Ogre::SceneNode* node, Ogre::Entity* ent, const Ogre::String& name)
	: m_node(node), m_entity(ent), m_objectName(name), m_essential(false)
{
	const auto parts = Ogre::StringUtil::split(name, "_");
	const size_t nameSize = parts.size();
	if (nameSize > 2)
	{
		const Ogre::String groupSep = parts[0];
		m_essential = !(Ogre::StringUtil::match(groupSep, "helper", false));
		m_tag = parts[1];
		m_id = parts[2];
	}
	else {
		m_tag = parts[0];
		m_id = parts[1];
		m_essential = true;
	}
}

WorldObject::~WorldObject()
{
	m_objectName.clear();
	m_tag.clear();
	m_id.clear();
}