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