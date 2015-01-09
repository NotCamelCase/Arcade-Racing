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

#include "World\WorldManager.h"

#include <OgreSceneManager.h>
#include <OgreLog.h>

#include "World\World.h"

#include "Utils.h"
#include "Game.h"

using namespace Ogre;

WorldManager::WorldManager(SceneManager* sm)
	: m_sceneMgr(sm), m_activeWorld(NULL), m_logger(NULL)
{
	m_logger = Game::getSingleton().getLog();
}

WorldManager::~WorldManager()
{
	SAFE_DELETE(m_activeWorld);

	m_logger->logMessage("WorldManager and current World destroyed");
}

World* WorldManager::loadWorld(const Ogre::String& trackID)
{
	m_activeWorld = new World(m_sceneMgr, trackID);
	m_activeWorld->load();
	
	return m_activeWorld;
}