#include "Screen\PlayerLoginScreen.h"

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#include "Screen\ScreenManager.h"
#include "Player\PlayerManager.h"
#include "Audio\AudioManager.h"
#include "Player\Player.h"
#include "Audio\Music.h"
#include "Game.h"

using namespace MyGUI;

PlayerLoginScreen::PlayerLoginScreen(ScreenManager* sm, ScreenTag tag)
	: Screen(sm, tag), m_bgImage(NULL), m_loginButton(NULL),
	m_usernameBox(NULL), m_passwordBox(NULL), m_bgMusic(NULL)
{
	m_uiFileName = "user_login.layout";
}

PlayerLoginScreen::~PlayerLoginScreen()
{
}

void PlayerLoginScreen::enter()
{
	m_ogrePlatform->getRenderManagerPtr()->setSceneManager(m_sceneMgr);

	static const Ogre::String resourceName = "Essential";
	if (!Ogre::ResourceGroupManager::getSingleton().isResourceGroupInitialised(resourceName))
		Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceName);
	if (!Ogre::ResourceGroupManager::getSingleton().isResourceGroupLoaded(resourceName))
		Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(resourceName);

	buildUI();
}

void PlayerLoginScreen::buildUI()
{
	m_guiLayout = LayoutManager::getInstance().loadLayout(m_uiFileName);

	m_bgImage = Gui::getInstance().findWidget<ImageBox>("background");
	m_bgImage->setSize(RenderManager::getInstance().getViewSize());
	m_bgImage->setImageTexture("blue_back.jpg");

	m_usernameBox = Gui::getInstance().findWidget<EditBox>("usernameBox");
	m_passwordBox = Gui::getInstance().findWidget<EditBox>("passwordBox");
	m_loginButton = Gui::getInstance().findWidget<Button>("loginButton");
	m_loginInfoLabel = Gui::getInstance().findWidget<TextBox>("infoLabel");

	if (PlayerManager::getSingleton().getLatestLocalPlayerName() != "")
	{
		const Ogre::String& latestPlayer = PlayerManager::getSingleton().getLatestLocalPlayerName();
		m_usernameBox->setCaption(latestPlayer); // Remember the latest local player's name
	}

	m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg", AudioType::SOUND_2D_LOOPING);
	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);

	m_loginButton->eventMouseButtonClick += newDelegate(this, &PlayerLoginScreen::buttonClicked);
}

void PlayerLoginScreen::update(Ogre::Real delta)
{
}

void PlayerLoginScreen::buttonClicked(MyGUI::WidgetPtr sender)
{
	if (sender == m_loginButton)
	{
		loginGame();
	}
}

void PlayerLoginScreen::loginGame()
{
	const UString& username = m_usernameBox->getCaption();
	const UString& password = m_passwordBox->getCaption();

	if (username.empty() || password.empty())
	{
		m_loginInfoLabel->setCaption("How am I to identify *you* among others ?!");

		return;
	}

	if (!PlayerManager::getSingleton().isLoginSuccessful(username, password))
	{
		m_loginInfoLabel->setCaption("Wrong user name and/or password. Retry!");

		return;
	}

	Game::getSingleton().getLog()->logMessage("Player " + username + " logged in");
	m_loginInfoLabel->setCaption("Login is successful.");

	m_screenManager->changeScreen(ScreenTag::TRACK_SELECT, false);
}

void PlayerLoginScreen::pause()
{
	Screen::pause();

	m_bgMusic->stop();
}

void PlayerLoginScreen::resume()
{
	Screen::resume();

	if (!m_bgMusic)
	{
		m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg", AudioType::SOUND_2D_LOOPING);
	}

	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);
}

void PlayerLoginScreen::leave()
{
	m_logger->logMessage("PlayerLoginScreen::leave()");

	if (m_guiLayout.size() > 0)
	{
		LayoutManager::getInstance().unloadLayout(m_guiLayout);
		m_guiLayout.clear();
	}

	m_bgMusic->stop();
}

bool PlayerLoginScreen::keyPressed(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyPress(KeyCode::Enum(e.key), e.text);

	return true;
}

bool PlayerLoginScreen::keyReleased(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyRelease(KeyCode::Enum(e.key));

	switch (e.key)
	{
	case OIS::KC_SPACE:
	case OIS::KC_RETURN:
		loginGame();
		break;
	case OIS::KC_ESCAPE:
		// Return to Main Menu
		m_screenManager->changeScreen(ScreenTag::MAIN_MENU, false);
		break;
	default:
		break;
	}

	return true;
}

bool PlayerLoginScreen::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMousePress(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}

bool PlayerLoginScreen::mouseMoved(const OIS::MouseEvent& e)
{
	InputManager::getInstance().injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);

	return true;
}

bool PlayerLoginScreen::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMouseRelease(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}