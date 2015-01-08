#include "Screen\OnlineLobbyScreen.h"

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#include "LoadBalancing-cpp\inc\Room.h"
#include "Common-cpp\inc\JVector.h"

#include "Game.h"
#include "Utils.h"
#include "Event\EventManager.h"
#include "Audio\AudioManager.h"
#include "Audio\SoundEffect.h"
#include "Screen\ScreenManager.h"
#include "Network\NetworkManager.h"

using namespace MyGUI;
using namespace ExitGames;

OnlineLobbyScreen::OnlineLobbyScreen(ScreenManager* sm, ScreenTag tag)
	: Screen(sm, tag), m_infoLabel(NULL), m_joinRandomButton(NULL), m_joinRoomButton(NULL), m_createRoomButton(NULL),
	m_hoverSound(NULL), m_clickSound(NULL)
{
	m_uiFileName = "online_lobby.layout";

	EventManager::getSingleton().registerListener(EventType::NET_CONNECTION_FAILED, this);
	EventManager::getSingleton().registerListener(EventType::NET_READY_IN_LOBBY, this);
	EventManager::getSingleton().registerListener(EventType::NET_WAITING_IN_ROOM, this);
	EventManager::getSingleton().registerListener(EventType::NET_READY_TO_JOIN_GAME, this);
}

OnlineLobbyScreen::~OnlineLobbyScreen()
{
}

void OnlineLobbyScreen::enter()
{
	m_ogrePlatform->getRenderManagerPtr()->setSceneManager(m_sceneMgr);

	static const Ogre::String resourceName = "Essential";
	if (!Ogre::ResourceGroupManager::getSingleton().isResourceGroupInitialised(resourceName))
		Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceName);
	if (!Ogre::ResourceGroupManager::getSingleton().isResourceGroupLoaded(resourceName))
		Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(resourceName);

	buildUI();

	NetworkManager::getSingleton().initLocalClient();
}

void OnlineLobbyScreen::buildUI()
{
	m_guiLayout = LayoutManager::getInstance().loadLayout(m_uiFileName);

	Gui::getInstance().findWidget<ImageBox>("background")->setSize(RenderManager::getInstance().getViewSize());

	m_createRoomButton = m_gui->findWidget<MyGUI::Button>("createRoomButton");
	m_joinRandomButton = m_gui->findWidget<MyGUI::Button>("joinRandomButton");
	m_joinRoomButton = m_gui->findWidget<MyGUI::Button>("joinRoomButton");

	m_createRoomButton->eventMouseButtonClick += MyGUI::newDelegate(this, &OnlineLobbyScreen::buttonClicked);
	m_joinRandomButton->eventMouseButtonClick += MyGUI::newDelegate(this, &OnlineLobbyScreen::buttonClicked);
	m_joinRoomButton->eventMouseButtonClick += MyGUI::newDelegate(this, &OnlineLobbyScreen::buttonClicked);

	m_createRoomButton->eventMouseSetFocus += MyGUI::newDelegate(this, &OnlineLobbyScreen::buttonHovered);
	m_joinRandomButton->eventMouseSetFocus += MyGUI::newDelegate(this, &OnlineLobbyScreen::buttonHovered);
	m_joinRoomButton->eventMouseSetFocus += MyGUI::newDelegate(this, &OnlineLobbyScreen::buttonHovered);

	m_infoLabel = m_gui->findWidget<MyGUI::TextBox>("infoLabel");

	m_clickSound = AudioManager::getSingleton().createSoundEffect("click.ogg");
	m_hoverSound = AudioManager::getSingleton().createSoundEffect("clickalt.ogg"); 
}

void OnlineLobbyScreen::buttonClicked(MyGUI::WidgetPtr sender)
{
	m_clickSound->play();

	if (sender == m_createRoomButton)
	{
		createRoomForPlayer();
	}
	else if (sender == m_joinRandomButton)
	{
		joinRandomRoom();
	}
	else if (sender == m_joinRoomButton)
	{
		joinSelectedRoom(0);
	}
}

void OnlineLobbyScreen::createRoomForPlayer()
{
	NetworkManager::getSingleton().createRoom();
}

void OnlineLobbyScreen::joinRandomRoom()
{
	NetworkManager::getSingleton().joinRandomRoom();
}

void OnlineLobbyScreen::joinSelectedRoom(int index)
{
	NetworkManager::getSingleton().joinRoom(index);
}

void OnlineLobbyScreen::buttonHovered(MyGUI::WidgetPtr sender, MyGUI::WidgetPtr old)
{
	m_hoverSound->play();
}

void OnlineLobbyScreen::onEvent(int eventType, void* data)
{
	switch (eventType)
	{
	case EventType::NET_CONNECTION_FAILED:
		deactivateLobbyUI();
		m_infoLabel->setCaption("Connection failed :(");
		break;
	case EventType::NET_READY_IN_LOBBY:
		activateLobbyUI();
		break;
	case EventType::NET_WAITING_IN_ROOM:
		deactivateLobbyUI();
		m_infoLabel->setCaption("Waiting for others to join the game...");
		break;
	case EventType::NET_READY_TO_JOIN_GAME:
		m_screenManager->changeScreen(ScreenTag::GAME_PLAY, true);
		break;
	default:
		break;
	}
}

void OnlineLobbyScreen::deactivateLobbyUI()
{
	m_createRoomButton->setVisible(false);
	m_joinRandomButton->setVisible(false);
	m_joinRoomButton->setVisible(false);
	m_infoLabel->setVisible(true);
}

void OnlineLobbyScreen::activateLobbyUI()
{
	m_infoLabel->setVisible(false);
	m_createRoomButton->setVisible(true);
	m_joinRandomButton->setVisible(true);
	m_joinRoomButton->setVisible(true);
}

void OnlineLobbyScreen::update(Ogre::Real delta)
{
}

void OnlineLobbyScreen::pause()
{
	Screen::pause();;
}

void OnlineLobbyScreen::resume()
{
	Screen::resume();
}

void OnlineLobbyScreen::leave()
{
	m_logger->logMessage("OnlineLobbyScreen::leave()");

	if (m_guiLayout.size() > 0)
	{
		LayoutManager::getInstance().unloadLayout(m_guiLayout);
		m_guiLayout.clear();
	}

	EventManager::getSingleton().unregisterListener(EventType::NET_CONNECTION_FAILED, this);
	EventManager::getSingleton().unregisterListener(EventType::NET_READY_IN_LOBBY, this);
	EventManager::getSingleton().unregisterListener(EventType::NET_WAITING_IN_ROOM, this);
	EventManager::getSingleton().unregisterListener(EventType::NET_READY_TO_JOIN_GAME, this);
}

bool OnlineLobbyScreen::keyPressed(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyPress(KeyCode::Enum(e.key), e.text);

	return true;
}

bool OnlineLobbyScreen::keyReleased(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyRelease(KeyCode::Enum(e.key));

	switch (e.key)
	{
	case OIS::KC_ESCAPE:
		NetworkManager::getSingleton().disconnect();
		m_screenManager->changeScreen(ScreenTag::MAIN_MENU, false);
		break;
	default:
		break;
	}

	return true;
}

bool OnlineLobbyScreen::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMousePress(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}

bool OnlineLobbyScreen::mouseMoved(const OIS::MouseEvent& e)
{
	InputManager::getInstance().injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);

	return true;
}

bool OnlineLobbyScreen::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMouseRelease(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}