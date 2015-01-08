#include "World\GameObject.h"

#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreLog.h>

#include "World\GameObjectManager.h"
#include "GODLoader.h"

std::map<Ogre::String, GameObjectType> GameObject::s_objectTypeMap = GameObject::creates_objectTypeMap();

GameObject::GameObject(GameObjectManager* mng, Ogre::SceneNode* node, Ogre::Entity* entity, const Ogre::String& id)
	: WorldObject(node, entity, id), m_manager(mng), m_logger(NULL)
{
	m_logger = mng->getLog();
}

GameObject::~GameObject()
{
	if (m_node && m_entity)
	{
		if (m_entity->isAttached())
			m_node->detachObject(m_entity);
		Ogre::SceneManager* sm = m_manager->getSceneManager();
		sm->destroyEntity(m_entity);
		m_entity = NULL;
	}
}

bool GameObject::operator==(GameObject* other)
{
	return (other->getType() == this->mType && other->getID() == this->m_id);
}

bool GameObject::operator!=(GameObject* other)
{
	return (other->getType() != this->mType && other->getID() != this->m_id);
}

int GameObject::getTypeByTag(const Ogre::String& tag)
{
	int type;
	auto it = s_objectTypeMap.find(tag);
	if (it == s_objectTypeMap.end())
	{
		type = GameObjectType::TYPE_STATIC;
	}
	else {
		type = (*it).second;
	}

	return type;
}

void GameObject::update(Ogre::Real delta)
{
}

std::map<Ogre::String, GameObjectType> GameObject::creates_objectTypeMap()
{
	std::map<Ogre::String, GameObjectType> mapping;
	mapping.insert(std::make_pair("vehicle", GameObjectType::TYPE_COMBAT_VEHICLE));
	mapping.insert(std::make_pair("wheel", GameObjectType::TYPE_WHEEL));
	mapping.insert(std::make_pair("checkpoint", GameObjectType::TYPE_CHECKPOINT));

	return mapping;
}