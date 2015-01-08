#pragma once

#include <map>

#include <OgreString.h>

#include "World\WorldObject.h"

namespace Ogre
{
	class Entity;
	class SceneNode;
	class Log;
}

enum GameObjectType
{
	TYPE_STATIC,
	TYPE_WHEEL,
	TYPE_CHECKPOINT,
	TYPE_COMBAT_VEHICLE
};

class GameObjectManager;

class GameObject : public WorldObject
{
public:
	GameObject(GameObjectManager* mng, Ogre::SceneNode* node, Ogre::Entity* entity, const Ogre::String& id);
	~GameObject();

	GameObjectType getType() const { return mType; }
	void setType(int t) { mType = GameObjectType(t); }

	bool operator==(GameObject* other);
	bool operator!=(GameObject* other);

	virtual void update(Ogre::Real delta);

	static int getTypeByTag(const Ogre::String& tag);

protected:
	GameObjectManager* m_manager;

	static std::map<Ogre::String, GameObjectType> creates_objectTypeMap();
	static std::map<Ogre::String, GameObjectType> s_objectTypeMap;

	GameObjectType mType;

	Ogre::Log* m_logger;
};