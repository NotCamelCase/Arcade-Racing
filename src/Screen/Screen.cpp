#include "Screen\Screen.h"

#include <Ogre.h>

#include <OISKeyboard.h>
#include <OISMouse.h>

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#include "Screen\ScreenManager.h"
#include "Game.h"

using namespace Ogre;
using namespace MyGUI;

Screen::Screen(ScreenManager *sm, ScreenTag tag)
	: m_screenManager(sm), m_sceneMgr(NULL), m_screenTag(tag),
	m_logger(NULL), m_camera(NULL), m_keyboard(NULL), m_mouse(NULL), m_gui(NULL),
	m_uiFileName(""), m_paused(false)
{
	m_logger = Game::getSingleton().getLog();

	static const String SM_TAG = "SceneManager";
	m_sceneMgr = Game::getSingleton().getRoot()->createSceneManager(
		SceneType::ST_GENERIC, SM_TAG + StringConverter::toString(tag));

	static const String CAM_TAG = "Camera";
	m_camera = m_sceneMgr->createCamera(CAM_TAG + StringConverter::toString(tag));

	m_camera->setAspectRatio(Real(Game::getSingleton().getViewport()->getActualWidth()) /
		Real(Game::getSingleton().getViewport()->getActualHeight()));
	Game::getSingleton().getViewport()->setCamera(m_camera);

	m_keyboard = m_screenManager->getKeyboard();
	m_mouse = m_screenManager->getMouse();

	m_ogrePlatform = ScreenManager::getOgrePlatform();
	m_gui = ScreenManager::getGUI();
}

Screen::~Screen()
{
	if (!m_guiLayout.empty())
		MyGUI::LayoutManager::getInstance().unloadLayout(m_guiLayout);
	m_guiLayout.clear();

	m_sceneMgr->clearScene();
	m_sceneMgr->destroyCamera(m_camera);
	Game::getSingleton().getRoot()->destroySceneManager(m_sceneMgr);
	m_logger->logMessage("Screen's SceneManager destroyed");
}

void Screen::pause()
{
	m_paused = true;
	// FIXME : Completely destroy GUI layout
	if (!m_guiLayout.empty())
	{
		MyGUI::LayoutManager::getInstance().unloadLayout(m_guiLayout);
		m_guiLayout.clear();
	}
}

void Screen::resume()
{
	m_paused = false;

	Game::getSingleton().getViewport()->setCamera(m_camera);
	m_ogrePlatform->getRenderManagerPtr()->setSceneManager(m_sceneMgr);

	buildUI();
}

void Screen::readjustMouseVisibility(bool setting)
{
	//ShowCursor(setting);
	MyGUI::PointerManager::getInstance().setVisible(setting);
}