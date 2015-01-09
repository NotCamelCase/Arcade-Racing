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

#include "Game.h"

#include <Ogre.h>

#include <OISInputManager.h>

#include "Utils.h"
#include "GODLoader.h"
#include "Audio\AudioManager.h"
#include "Event\EventManager.h"
#include "Player\RaceManager.h"
#include "Screen\ScreenManager.h"
#include "Player\PlayerManager.h"
#include "Network\NetworkManager.h"

// Memory leak detect etmek içün malloc ve free debug mode da çağrılıyor
// exit point te memory dump edilip rapor veriyor
//#define PROFILE_MEMORY_USAGE

#if defined(PROFILE_MEMORY_USAGE)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#if _DEBUG
#define CONSOLE_OUTPUT
#endif

#if _DEBUG
const unsigned int GAME_TICK_MS = 16;
#else
const unsigned int GAME_TICK_MS = 8;
#endif

#define GAME_READY (!_DEBUG && 0)

// Initialize the game mode to Practice as default
GameMode Game::s_currentGameMode = GameMode::PRACTICE;
DetailMode Game::s_shadowQuality = DetailMode::MEDIUM;
DetailMode Game::s_textureQuality = DetailMode::MEDIUM;

Game::Game()
	: m_root(NULL), m_window(NULL), m_logger(NULL), m_viewport(NULL),
	m_inputMgr(NULL), m_logMgr(NULL), m_screenManager(NULL), m_audioManager(NULL), m_eventManager(NULL),
	m_playerManager(NULL), m_raceManager(NULL), m_godLoader(NULL), m_networkManager(NULL),
	m_pluginsCfg(""), m_resCfg(""), m_ogreCfg("")
{
#if _DEBUG
	m_pluginsCfg = "plugins_d.cfg";
#else
	m_pluginsCfg = "plugins.cfg";
#endif

	m_resCfg = "resources.cfg";
	m_ogreCfg = "Ogre.cfg";

#if _DEBUG
	bool logOutput = true;
	bool suppressLog = false;
#else
	bool logOutput = false;
	bool suppressLog = true;
#endif

	m_logMgr = new Ogre::LogManager();
	m_logMgr->createLog("GameEngine.log", true, false, false);

	m_logger = m_logMgr->createLog("RaceSim.log", false, logOutput, suppressLog);
	m_logger->logMessage("Game started off");
}

Game::~Game()
{
	SAFE_DELETE(m_screenManager);
	SAFE_DELETE(m_networkManager);
	SAFE_DELETE(m_audioManager);
	SAFE_DELETE(m_eventManager);
	SAFE_DELETE(m_playerManager);
	SAFE_DELETE(m_raceManager);
	SAFE_DELETE(m_godLoader);
	SAFE_DELETE(m_root);
	m_logger->logMessage("Game destroyed");

	m_logMgr->destroyLog(m_logger);
	SAFE_DELETE(m_logMgr);
}

Game& Game::getSingleton()
{
	static Game oneAndOnlyInstance;

	return oneAndOnlyInstance;
}

bool Game::run()
{
	m_root = new Ogre::Root(m_pluginsCfg, m_ogreCfg);
	if (!(m_root->restoreConfig() || m_root->showConfigDialog()))
	{
		m_logger->logMessage("Ogre could not be created or restored");

		return false;
	}

	m_window = m_root->initialise(true, "Grad Project - Race Sim");
	m_logger->logMessage("Render window created");

	m_viewport = m_window->addViewport(NULL);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	readyResources();

	m_logger->logMessage("Initializing OIS");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
	m_window->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#if !GAME_READY
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#endif
	m_inputMgr = OIS::InputManager::createInputSystem(pl);

	m_godLoader = &GODLoader::getSingleton();
	m_audioManager = &AudioManager::getSingleton(); //m_audioManager->muteAudio();
	m_eventManager = &EventManager::getSingleton();
	m_raceManager = &RaceManager::getSingleton();
	m_playerManager = &PlayerManager::getSingleton();
	m_networkManager = &NetworkManager::getSingleton();
	m_screenManager = new ScreenManager(m_inputMgr);

	ShowCursor(false); // Hide native Windows cursor!
	m_window->setActive(true);

	m_logger->logMessage("Game ready to start rendering");

	Ogre::Timer gameTimer;
	unsigned long gameClockTime;
	gameTimer.reset();
	gameClockTime = gameTimer.getMilliseconds();

	bool runGame = true;
	while (runGame)
	{
		auto dt = gameTimer.getMilliseconds() - gameClockTime;
		while (dt >= GAME_TICK_MS)
		{
			Ogre::WindowEventUtilities::messagePump();
			runGame = m_root->renderOneFrame();

			if (s_currentGameMode == GameMode::MULTI_PLAYER)
				m_networkManager->update((float) dt);

			m_audioManager->update();

			dt -= GAME_TICK_MS;
			gameClockTime += GAME_TICK_MS;
		}
	}

	return true;
}

void Game::readyResources()
{
	m_logger->logMessage("Game::readyResources()");

	Ogre::String secName, typeName, absName;
	Ogre::ConfigFile cf;
	cf.load(m_resCfg);

	Ogre::ConfigFile::SectionIterator it = cf.getSectionIterator();
	while (it.hasMoreElements())
	{
		secName = it.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = it.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); i++)
		{
			typeName = i->first;
			absName = i->second;
			Ogre::ResourceGroupManager::getSingletonPtr()->addResourceLocation(absName, typeName, secName);
		}
	}
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Essential");

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	m_logger->logMessage("Resources readied");
}


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
	int main(int argc, char *argv[])
#endif
	{
#ifdef CONSOLE_OUTPUT
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen("CON", "w", stdout);
#endif

		try {
			Game::getSingleton().run();
		}
		catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
		}

#ifdef CONSOLE_OUTPUT
		FreeConsole();
#endif

#ifdef PROFILE_MEMORY_USAGE
		_CrtDumpMemoryLeaks();
#endif

		return 0;
	}

#ifdef __cplusplus
}
#endif