#include "Screen\TrackSelectScreen.h"

#include "Screen\ScreenManager.h"

#include <Ogre.h>

#include <OISMouse.h>
#include <OISKeyboard.h>

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#include <MyGUI_WidgetDefines.h>

#include "Game.h"
#include "Utils.h"
#include "GODLoader.h"
#include "Player\Player.h"
#include "Audio\Music.h"
#include "Audio\SoundEffect.h"
#include "Player\RaceManager.h"
#include "Player\PlayerManager.h"
#include "Audio\AudioManager.h"

using namespace MyGUI;
using namespace Ogre;

TrackSelectScreen::TrackSelectScreen(ScreenManager* sm, ScreenTag tag)
	: Screen(sm, tag), m_playerManager(NULL), m_prevButton(NULL), m_nextButton(NULL),
	m_selectButton(NULL), m_backButton(NULL), m_clickSE(NULL), m_currentTrackIndex(0),
	m_bgMusic(NULL), m_hoverSE(NULL)
{
	m_playerManager = &PlayerManager::getSingleton();

	m_uiFileName = "track_select.layout";
}

TrackSelectScreen::~TrackSelectScreen()
{
}

void TrackSelectScreen::enter()
{
	m_logger->logMessage("TrackSelectScreen::enter()");

	m_ogrePlatform->getRenderManagerPtr()->setSceneManager(m_sceneMgr);

	static const String resourceName = "Essential";
	if (!ResourceGroupManager::getSingleton().isResourceGroupInitialised(resourceName))
		ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceName);
	if (!ResourceGroupManager::getSingleton().isResourceGroupLoaded(resourceName))
		ResourceGroupManager::getSingleton().loadResourceGroup(resourceName);

	const String& trackNames = GODLoader::getSingleton().getGameObjectDefinitionByTag("track_list")->getParameters()["tracks"];
	m_trackList = StringUtil::split(trackNames, " ");

	m_clickSE = AudioManager::getSingleton().createSoundEffect("click.ogg");
	m_hoverSE = AudioManager::getSingleton().createSoundEffect("clickalt.ogg");

	buildUI();
}

void TrackSelectScreen::buildUI()
{
	m_logger->logMessage("TrackSelectScreen::buildUI()");

	m_guiLayout = LayoutManager::getInstancePtr()->loadLayout(m_uiFileName);

	ImageBox* parentBG = Gui::getInstance().findWidget<ImageBox>("background");
	parentBG->setSize(RenderManager::getInstance().getViewSize());
	parentBG->setImageTexture("blue_back.jpg");

	m_trackPreview = Gui::getInstance().findWidget<ImageBox>("trackImage");

	m_trackNiceName = Gui::getInstance().findWidget<TextBox>("trackNameLabel");
	m_trackLaps = Gui::getInstance().findWidget<TextBox>("lapLabel");

	m_nextButton = Gui::getInstance().findWidget<ImageBox>("nextButton");
	m_nextButton->setImageTexture("next.png");

	m_prevButton = Gui::getInstance().findWidget<ImageBox>("prevButton");
	m_prevButton->setImageTexture("prev.png");

	m_backButton = Gui::getInstance().findWidget<Button>("backButton");
	m_selectButton = Gui::getInstance().findWidget<Button>("trackSelectButton");

	m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg", AudioType::SOUND_2D_LOOPING);
	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);

	m_prevButton->eventMouseButtonClick += newDelegate(this, &TrackSelectScreen::buttonClicked);
	m_backButton->eventMouseButtonClick += newDelegate(this, &TrackSelectScreen::buttonClicked);
	m_selectButton->eventMouseButtonClick += newDelegate(this, &TrackSelectScreen::buttonClicked);
	m_nextButton->eventMouseButtonClick += newDelegate(this, &TrackSelectScreen::buttonClicked);

	m_prevButton->eventMouseSetFocus += newDelegate(this, &TrackSelectScreen::buttonHovered);
	m_backButton->eventMouseSetFocus += newDelegate(this, &TrackSelectScreen::buttonHovered);
	m_selectButton->eventMouseSetFocus += newDelegate(this, &TrackSelectScreen::buttonHovered);
	m_nextButton->eventMouseSetFocus += newDelegate(this, &TrackSelectScreen::buttonHovered);

	inflateTrackInfo();
}

void TrackSelectScreen::buttonHovered(WidgetPtr sender, WidgetPtr old)
{
	m_hoverSE->play();
}

void TrackSelectScreen::nextTrackClicked()
{
	m_logger->logMessage("TrackSelectScreen::nextTrackClicked()");

	static const size_t length = m_trackList.size();
	if (m_currentTrackIndex + 1 < length)
		m_currentTrackIndex++;
	else
		m_currentTrackIndex = 0;

	inflateTrackInfo();
}

void TrackSelectScreen::prevTrackClicked()
{
	m_logger->logMessage("TrackSelectScreen::prevTrackClicked()");

	if (m_currentTrackIndex - 1 >= 0)
		m_currentTrackIndex--;
	else
	{
		static const size_t length = m_trackList.size();
		m_currentTrackIndex = length - 1;
	}

	inflateTrackInfo();
}

void TrackSelectScreen::backToMenuClicked()
{
	m_logger->logMessage("TrackSelectScreen::backToMenuClicked()");

	m_screenManager->changeScreen(ScreenTag::MAIN_MENU, false);
}

void TrackSelectScreen::selectTrackClicked()
{
	m_logger->logMessage("TrackSelectScreen::selectTrackClicked()");

	RaceManager::getSingleton().createTrackData(m_trackList[m_currentTrackIndex]);

	m_screenManager->changeScreen(ScreenTag::VEHICLE_SELECT, true);
}

void TrackSelectScreen::update(Ogre::Real delta)
{
}

void TrackSelectScreen::buttonClicked(WidgetPtr sender)
{
	m_clickSE->play();
	if (sender == m_backButton)
		backToMenuClicked();
	else if (sender == m_selectButton)
		selectTrackClicked();
	else if (sender == m_nextButton)
		nextTrackClicked();
	else if (sender == m_prevButton)
		prevTrackClicked();
}

void TrackSelectScreen::resume()
{
	Screen::resume();

	if (!m_bgMusic)
	{
		m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg");
	}

	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);
}

void TrackSelectScreen::pause()
{
	Screen::pause();

	m_bgMusic->stop();
}

void TrackSelectScreen::leave()
{
	if (m_guiLayout.size() > 0)
	{
		LayoutManager::getInstance().unloadLayout(m_guiLayout);
		m_guiLayout.clear();
	}

	m_bgMusic->stop();
	//AudioManager::getSingleton().release(m_bgMusic);
}

bool TrackSelectScreen::keyPressed(const OIS::KeyEvent& e)
{
	InputManager::getInstancePtr()->injectKeyPress(KeyCode::Enum(e.key), e.text);

	return true;
}

bool TrackSelectScreen::keyReleased(const OIS::KeyEvent& e)
{
	InputManager::getInstancePtr()->injectKeyRelease(KeyCode::Enum(e.key));

	switch (e.key)
	{
	case OIS::KC_RETURN:
		selectTrackClicked();
		break;
	case OIS::KC_LEFT:
		prevTrackClicked();
		break;
	case OIS::KC_RIGHT:
		nextTrackClicked();
		break;
	default:
		break;
	}

	return true;
}

bool TrackSelectScreen::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstancePtr()->injectMousePress(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}

bool TrackSelectScreen::mouseMoved(const OIS::MouseEvent& e)
{
	InputManager::getInstancePtr()->injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);

	return true;
}

bool TrackSelectScreen::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstancePtr()->injectMouseRelease(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}

bool TrackSelectScreen::inflateTrackInfo()
{
	m_logger->logMessage("TrackSelectScreen::inflateTrackInfo()");

	GOD* trackGOD = GODLoader::getSingleton().getGameObjectDefinitionByTag(m_trackList[m_currentTrackIndex]);
	if (!trackGOD) return false;

	auto& defs = trackGOD->getParameters();
	m_trackPreview->setImageTexture(defs["preview"]);
	m_trackNiceName->setCaption(defs["nice_name"]);
	m_trackLaps->setCaption(defs["lap_number"]);

	return true;
}