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