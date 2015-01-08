#include "Screen\ScreenManager.h"

#include <Ogre.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#include "Game.h"
#include "Utils.h"
#include "Screen\CreditsScreen.h"
#include "Screen\MainMenuScreen.h"
#include "Screen\GamePlayScreen.h"
#include "Screen\OnlineLobbyScreen.h"
#include "Screen\TrackSelectScreen.h"
#include "Screen\PlayerLoginScreen.h"
#include "Screen\VehicleSelectScreen.h"

MyGUI::OgrePlatform* ScreenManager::m_ogrePlatform = NULL;
MyGUI::Gui* ScreenManager::m_gui = NULL;

ScreenManager::ScreenManager(OIS::InputManager *im)
	: m_game(NULL), m_inputMgr(im), m_keyboard(NULL),
	m_mouse(NULL), m_activeScreen(NULL), m_logger(NULL), m_shutDown(false)
{
	m_game = &Game::getSingleton();

	m_logger = m_game->getLog();
	m_logger->logMessage("Initializing ScreenManager");

	m_logger->logMessage("Keyboard and mouse setting up");
	m_keyboard = static_cast<OIS::Keyboard*> (m_inputMgr->createInputObject(OIS::Type::OISKeyboard, true));
	m_mouse = static_cast<OIS::Mouse*> (m_inputMgr->createInputObject(OIS::Type::OISMouse, true));

	m_mouse->getMouseState().width = m_game->getWindow()->getWidth();
	m_mouse->getMouseState().height = m_game->getWindow()->getHeight();

	m_game->getRoot()->addFrameListener(this);
	Ogre::WindowEventUtilities::addWindowEventListener(m_game->getWindow(), this);

	m_game->getViewport()->setBackgroundColour(Ogre::ColourValue(.5, .5, .5));

	m_ogrePlatform = new MyGUI::OgrePlatform();
	m_activeScreen = new MainMenuScreen(this, ScreenTag::MAIN_MENU);
	m_screens.push_back(m_activeScreen);
	m_ogrePlatform->initialise(m_game->getWindow(), m_activeScreen->getSceneManager(), "Essential");
	m_gui = new MyGUI::Gui();
	m_gui->initialise("MyGUI_Core.xml");

	m_keyboard->setEventCallback(m_activeScreen);
	m_mouse->setEventCallback(m_activeScreen);

	m_activeScreen->enter();
}

ScreenManager::~ScreenManager()
{
	m_logger->logMessage("Destroying ScreenManager");
	Ogre::WindowEventUtilities::removeWindowEventListener(m_game->getWindow(), this);
	m_game->getRoot()->removeFrameListener(this);

	m_ogrePlatform->getRenderManagerPtr()->setSceneManager(NULL);

	m_logger->logMessage("Destroying game screens");
	for (std::vector<Screen*>::iterator it = m_screens.begin(); it != m_screens.end(); ++it)
	{
		(*it)->leave();
		SAFE_DELETE(*it);
	}
	m_screens.clear();

	m_activeScreen = NULL;

	m_gui->shutdown();
	SAFE_DELETE(m_gui);

	m_ogrePlatform->shutdown();
	SAFE_DELETE(m_ogrePlatform);
	m_logger->logMessage("MyGUI destroyed");

	windowClosed(m_game->getWindow());
}

Screen* ScreenManager::getScreen(ScreenTag tag)
{
	Screen* gameScreen = NULL;
	for (Screen* gs : m_screens)
	{
		if (gs->getScreenTag() == tag)
		{
			gameScreen = gs;

			return gameScreen;
		}
	}

	return gameScreen;
}

void ScreenManager::changeScreen(ScreenTag tag, bool pausePrevious)
{
	assert(m_activeScreen->getScreenTag() != tag && "Invalid request to change the current active screen to the same!");

	m_keyboard->setEventCallback(NULL);
	m_mouse->setEventCallback(NULL);

	if (pausePrevious)
	{
		m_activeScreen->pause();
	}
	else
	{
		m_ogrePlatform->getRenderManagerPtr()->setSceneManager(NULL);
		m_activeScreen->leave();
		SAFE_DELETE(m_screens.back());
		m_screens.pop_back();
	}

	Screen* nextScreen = getScreen(tag);
	if (nextScreen == NULL) // Screen not created early or destroyed on-purpose
	{
		switch (tag)
		{
		case ScreenTag::MAIN_MENU:
			nextScreen = new MainMenuScreen(this, tag);
			break;
		case ScreenTag::GAME_PLAY:
			nextScreen = new GamePlayScreen(this, tag);
			break;
		case ScreenTag::TRACK_SELECT:
			nextScreen = new TrackSelectScreen(this, tag);
			break;
		case ScreenTag::VEHICLE_SELECT:
			nextScreen = new VehicleSelectScreen(this, tag);
			break;
		case ScreenTag::USER_LOGIN:
			nextScreen = new PlayerLoginScreen(this, tag);
			break;
		case ScreenTag::GAME_LOBBY:
			nextScreen = new OnlineLobbyScreen(this, tag);
			break;
		case ScreenTag::CREDITS:
			nextScreen = new CreditsScreen(this, tag);
			break;
		default:
			break;
		}

		addScreen(nextScreen);
		m_activeScreen = nextScreen;
		m_activeScreen->enter();
	}
	else {
		m_activeScreen = nextScreen;
		if (m_activeScreen->isPaused())
		{
			m_activeScreen->resume();
		}
	}

	m_keyboard->setEventCallback(m_activeScreen);
	m_mouse->setEventCallback(m_activeScreen);
}

bool ScreenManager::addScreen(Screen* screen)
{
	bool res = (screen != NULL);
	m_screens.push_back(screen);

	return res;
}

void ScreenManager::destroyDeactiveScreens()
{
	for (auto it = m_screens.begin(); it != m_screens.end();)
	{
		Screen* cs = (*it);
		if (cs && cs != m_activeScreen)
		{
			SAFE_DELETE(cs);
			it = m_screens.erase(it++);
		}
		else {
			++it;
		}
	}

	m_logger->logMessage("Deactive screens popped");
}

bool ScreenManager::frameStarted(const Ogre::FrameEvent &e)
{
	return !m_shutDown;
}

bool ScreenManager::frameRenderingQueued(const Ogre::FrameEvent &e)
{
	m_keyboard->capture();
	m_mouse->capture();

	m_activeScreen->update(e.timeSinceLastFrame);

	return true;
}

bool ScreenManager::frameEnded(const Ogre::FrameEvent &e)
{
	return true;
}

void ScreenManager::windowResized(Ogre::RenderWindow* rw)
{
	m_mouse->getMouseState().width = rw->getWidth();
	m_mouse->getMouseState().height = rw->getHeight();
}

bool ScreenManager::windowClosing(Ogre::RenderWindow* rw)
{
	m_shutDown = true;

	return m_shutDown;
}

void ScreenManager::windowClosed(Ogre::RenderWindow *rw)
{
	if (rw == m_game->getWindow())
	{
		if (m_inputMgr)
		{
			m_inputMgr->destroyInputObject(m_keyboard);
			m_inputMgr->destroyInputObject(m_mouse);
			OIS::InputManager::destroyInputSystem(m_inputMgr);

			m_inputMgr = NULL;
			m_keyboard = NULL;
			m_mouse = NULL;
			m_logger->logMessage("OIS Objects deleted");
		}
	}
}