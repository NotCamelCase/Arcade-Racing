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