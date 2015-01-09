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

#include "Screen\CreditsScreen.h"

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#include <fmod.hpp>

#include "Screen\ScreenManager.h"
#include "Audio\AudioManager.h"
#include "Audio\Music.h"
#include "Utils.h"
#include "Game.h"

using namespace Ogre;
using namespace MyGUI;

CreditsScreen::CreditsScreen(ScreenManager* sm, ScreenTag tag)
	: Screen(sm, tag), m_bgImage(NULL), m_bgMusic(NULL)
{
	m_uiFileName = "credits.layout";
}

CreditsScreen::~CreditsScreen()
{
}

void CreditsScreen::enter()
{
	m_ogrePlatform->getRenderManagerPtr()->setSceneManager(m_sceneMgr);

	static const String resourceName = "Essential";
	if (!ResourceGroupManager::getSingleton().isResourceGroupInitialised(resourceName))
		ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceName);
	if (!ResourceGroupManager::getSingleton().isResourceGroupLoaded(resourceName))
		ResourceGroupManager::getSingleton().loadResourceGroup(resourceName);

	buildUI();
}

void CreditsScreen::buildUI()
{
	m_guiLayout = LayoutManager::getInstance().loadLayout(m_uiFileName);
	assert(!m_guiLayout.empty() && "Main Menu UI failed to load!");

	m_bgImage = Gui::getInstance().findWidget<ImageBox>("background");
	m_bgImage->setSize(RenderManager::getInstance().getViewSize());
	//m_bgImage->setImageTexture("race_back.png");

	m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg", AudioType::SOUND_2D_LOOPING);

	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);
}

void CreditsScreen::update(Real delta)
{
}

void CreditsScreen::pause()
{
	Screen::pause();

	m_bgMusic->stop();
}

void CreditsScreen::resume()
{
	Screen::resume();

	if (!m_bgMusic)
	{
		m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg", AudioType::SOUND_2D_LOOPING);
	}

	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);
}

void CreditsScreen::leave()
{
	m_logger->logMessage("CreditsScreen::leave()");

	if (m_guiLayout.size() > 0)
	{
		LayoutManager::getInstance().unloadLayout(m_guiLayout);
		m_guiLayout.clear();
	}
}

bool CreditsScreen::keyPressed(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyPress(KeyCode::Enum(e.key), e.text);

	return true;
}

bool CreditsScreen::keyReleased(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyRelease(KeyCode::Enum(e.key));

	switch (e.key)
	{
	case OIS::KC_ESCAPE:
		m_screenManager->changeScreen(ScreenTag::MAIN_MENU, true);
		break;
	default:
		break;
	}

	return true;
}

bool CreditsScreen::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMousePress(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}

bool CreditsScreen::mouseMoved(const OIS::MouseEvent& e)
{
	InputManager::getInstance().injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);

	return true;
}

bool CreditsScreen::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMouseRelease(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}