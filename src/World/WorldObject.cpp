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