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