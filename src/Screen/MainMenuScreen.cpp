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

#include "Screen\MainMenuScreen.h"

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#include <fmod.hpp>

#include "Screen\ScreenManager.h"
#include "Player\PlayerManager.h"
#include "Audio\AudioManager.h"
#include "Audio\SoundEffect.h"
#include "Player\Player.h"
#include "Audio\Music.h"
#include "Utils.h"
#include "Game.h"

using namespace Ogre;
using namespace MyGUI;

#define SKIP_TO_GAMEPLAY // Skip directly to GameplayScreen pressing SPACE when developing
//#undefine

MainMenuScreen::MainMenuScreen(ScreenManager* sm, ScreenTag tag)
	: Screen(sm, tag), m_singlePlayerButton(NULL), m_multiplayerButton(NULL),
	m_exitButton(NULL), m_creditsButton(NULL), m_bgImage(NULL),
	m_clickSound(NULL), m_bgMusic(NULL), m_hoverSound(NULL)
{
	m_uiFileName = "main_menu.layout";
}

MainMenuScreen::~MainMenuScreen()
{
}

void MainMenuScreen::enter()
{
	m_ogrePlatform->getRenderManagerPtr()->setSceneManager(m_sceneMgr);

	static const String resourceName = "Essential";
	if (!ResourceGroupManager::getSingleton().isResourceGroupInitialised(resourceName))
		ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceName);
	if (!ResourceGroupManager::getSingleton().isResourceGroupLoaded(resourceName))
		ResourceGroupManager::getSingleton().loadResourceGroup(resourceName);

	buildUI();
}

void MainMenuScreen::buildUI()
{
	m_guiLayout = LayoutManager::getInstance().loadLayout(m_uiFileName);
	assert(!m_guiLayout.empty() && "Main Menu UI failed to load!");

	m_bgImage = Gui::getInstance().findWidget<ImageBox>("background");
	m_bgImage->setSize(RenderManager::getInstance().getViewSize());
	m_bgImage->setImageTexture("race_back.png");

	m_singlePlayerButton = Gui::getInstance().findWidget<Button>("singleplayerButton");
	m_multiplayerButton = Gui::getInstance().findWidget<Button>("multiplayerButton");
	m_exitButton = Gui::getInstance().findWidget<Button>("exitButton");
	m_creditsButton = Gui::getInstance().findWidget<Button>("creditsButton");

	m_singlePlayerButton->setNeedMouseFocus(true);
	m_singlePlayerButton->eventMouseSetFocus += newDelegate(this, &MainMenuScreen::buttonHovered);
	m_singlePlayerButton->eventMouseButtonClick += newDelegate(this, &MainMenuScreen::buttonClicked);

	m_multiplayerButton->setNeedMouseFocus(true);
	m_multiplayerButton->eventMouseSetFocus += newDelegate(this, &MainMenuScreen::buttonHovered);
	m_multiplayerButton->eventMouseButtonClick += newDelegate(this, &MainMenuScreen::buttonClicked);

	m_exitButton->setNeedMouseFocus(true);
	m_exitButton->eventMouseSetFocus += newDelegate(this, &MainMenuScreen::buttonHovered);
	m_exitButton->eventMouseButtonClick += newDelegate(this, &MainMenuScreen::buttonClicked);

	m_creditsButton->setNeedMouseFocus(true);
	m_creditsButton->eventMouseSetFocus += newDelegate(this, &MainMenuScreen::buttonHovered);
	m_creditsButton->eventMouseButtonClick += newDelegate(this, &MainMenuScreen::buttonClicked);

	m_clickSound = AudioManager::getSingleton().createSoundEffect("click.ogg");
	m_hoverSound = AudioManager::getSingleton().createSoundEffect("clickalt.ogg");
	m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg", AudioType::SOUND_2D_LOOPING);
	
	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);
}

void MainMenuScreen::update(Real delta)
{
}

void MainMenuScreen::pause()
{
	Screen::pause();

	m_bgMusic->stop();
}

void MainMenuScreen::resume()
{
	Screen::resume();

	if (!m_bgMusic)
	{
		m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg", AudioType::SOUND_2D_LOOPING);
	}

	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);
}

void MainMenuScreen::leave()
{
	m_logger->logMessage("MainMenuScreen::leave()");

	if (m_guiLayout.size() > 0)
	{
		LayoutManager::getInstance().unloadLayout(m_guiLayout);
		m_guiLayout.clear();
	}

	//AudioManager::getSingleton().release(m_clickSound);
	//AudioManager::getSingleton().release(m_hoverSound);
	//AudioManager::getSingleton().release(m_bgMusic);
}

void MainMenuScreen::buttonClicked(WidgetPtr sender)
{
	if (sender == m_singlePlayerButton)
		enterPracticeMode();
	else if (sender == m_multiplayerButton)
		enterMultiplayer();
	else if (sender == m_creditsButton)
		showCredits();
	else if (sender == m_exitButton)
		exitGame();
}

void MainMenuScreen::buttonHovered(WidgetPtr sender, WidgetPtr old)
{
	m_hoverSound->play();
}

void MainMenuScreen::enterPracticeMode()
{
	m_logger->logMessage("MainMenuScreen::enterPracticeMode()");
	m_clickSound->play();

	Game::setGameMode(GameMode::PRACTICE);

	if (!PlayerManager::getSingleton().getLocalPlayer() || !PlayerManager::getSingleton().getLocalPlayer()->isLoggedIn())
	{
		m_screenManager->changeScreen(ScreenTag::USER_LOGIN, false);
	}
	else
	{
		m_screenManager->changeScreen(ScreenTag::TRACK_SELECT, false);
	}
}

void MainMenuScreen::enterMultiplayer()
{
	m_logger->logMessage("MainMenuScreen::enterOnlineMode()");
	m_clickSound->play();

	Game::setGameMode(GameMode::MULTI_PLAYER); 

	if (!PlayerManager::getSingleton().getLocalPlayer() || !PlayerManager::getSingleton().getLocalPlayer()->isLoggedIn())
	{
		m_screenManager->changeScreen(ScreenTag::USER_LOGIN, false);
	}
	else
	{
		m_screenManager->changeScreen(ScreenTag::GAME_LOBBY, false);
	}
}

void MainMenuScreen::showCredits()
{
	m_logger->logMessage("MainMenuScreen::showCredits()");
	m_clickSound->play();

	m_screenManager->changeScreen(ScreenTag::CREDITS, true);
}

void MainMenuScreen::exitGame()
{
	m_logger->logMessage("MainMenuScreen::exitGame()");
	m_clickSound->play();
	m_screenManager->setShutDown(true);
}

bool MainMenuScreen::keyPressed(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyPress(KeyCode::Enum(e.key), e.text);

	switch (e.key)
	{
	case OIS::KC_ESCAPE:
		m_screenManager->setShutDown(true);
		break;
	}

	return true;
}

bool MainMenuScreen::keyReleased(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyRelease(KeyCode::Enum(e.key));

#if defined (SKIP_TO_GAMEPLAY)
	switch (e.key)
	{
	// DEBUG ONLY !!!
	case OIS::KC_SPACE:
		AudioManager::getSingleton().release(m_clickSound);
		AudioManager::getSingleton().release(m_hoverSound);
		AudioManager::getSingleton().release(m_bgMusic);
		m_screenManager->changeScreen(ScreenTag::GAME_PLAY, false);
		break;
	default:
		break;
	}
#endif

	return true;
}

bool MainMenuScreen::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMousePress(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}

bool MainMenuScreen::mouseMoved(const OIS::MouseEvent& e)
{
	InputManager::getInstance().injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);

	return true;
}

bool MainMenuScreen::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMouseRelease(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}