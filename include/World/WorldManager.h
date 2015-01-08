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