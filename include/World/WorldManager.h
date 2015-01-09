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

namespace Ogre { class SceneManager; class Log; }

class Game;
class World;

class WorldManager
{
public:
	WorldManager(Ogre::SceneManager* sm);
	~WorldManager();

	friend class Game;

	World* loadWorld(const Ogre::String& WorldID);

	World* getCurrentWorld() const { return m_activeWorld; }
	void setCurrentWorld(World* activeWorld) { m_activeWorld = activeWorld; }

private:
	World* m_activeWorld;

	Ogre::Log* m_logger;
	Ogre::SceneManager* m_sceneMgr;
};