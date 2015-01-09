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