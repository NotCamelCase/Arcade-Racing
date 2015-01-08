#pragma once

#include <OgreString.h>

namespace Ogre { class Root; class RenderWindow; class Viewport; class Log; class LogManager; }
namespace OIS { class InputManager; }

class NetworkManager;
class ScreenManager;
class PlayerManager;
class AudioManager;
class EventManager;
class RaceManager;
class GODLoader;

enum class GameMode { PRACTICE = 1, MULTI_PLAYER };
enum class DetailMode { LOW, MEDIUM, HIGH };

/*
* The entry point that runs game loop, handles specific managers and also Ogre stuff
* such as @Ogre::Root and @Ogre::Window
*/
class Game
{
public:
	Ogre::Root* getRoot() const { return m_root; }
	Ogre::LogManager* getLogManager() const { return m_logMgr; }
	Ogre::RenderWindow* getWindow() const { return m_window; }
	Ogre::Viewport* getViewport() const { return m_viewport; }
	Ogre::Log* getLog() const { return m_logger; }
	
	static GameMode getGameMode() { return s_currentGameMode; }
	static void setGameMode(GameMode mode) { s_currentGameMode = mode; }

	static DetailMode getTextureQuality() { return s_textureQuality; }
	static DetailMode getShadowQuality() { return s_shadowQuality; }

	static void setTextureQuality(DetailMode dm) { s_textureQuality = dm; }
	static void setShadowQuality(DetailMode dm) { s_shadowQuality = dm; }

	bool run();

	static Game& getSingleton();

private:
	Game();
	~Game();

	void readyResources();

	Ogre::Root* m_root;
	Ogre::LogManager* m_logMgr;
	Ogre::Log* m_logger;
	Ogre::RenderWindow* m_window;
	Ogre::Viewport* m_viewport;

	OIS::InputManager* m_inputMgr;

	Ogre::String m_pluginsCfg;
	Ogre::String m_ogreCfg;
	Ogre::String m_resCfg;

	RaceManager* m_raceManager;
	ScreenManager* m_screenManager;
	NetworkManager* m_networkManager;
	AudioManager* m_audioManager;
	EventManager* m_eventManager;
	PlayerManager* m_playerManager;
	GODLoader* m_godLoader;
	
	static GameMode s_currentGameMode;
	static DetailMode s_textureQuality;
	static DetailMode s_shadowQuality;
};