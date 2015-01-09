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

#include "Screen\VehicleSelectScreen.h"

#include <Ogre.h>

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#include "Screen\ScreenManager.h"
#include "Player\PlayerManager.h"
#include "Player\Player.h"
#include "Audio\AudioManager.h"
#include "Audio\SoundEffect.h"
#include "Audio\Music.h"
#include "Game.h"
#include "GODLoader.h"
#include "Utils.h"

using namespace Ogre;
using namespace MyGUI;

VehicleSelectScreen::VehicleSelectScreen(ScreenManager* sm, ScreenTag tag)
	: Screen(sm, tag), m_prevBtn(NULL), m_nextBtn(NULL),
	m_backBtn(NULL), m_selectBtn(NULL), m_maxSpeedProgress(NULL), m_accelProgress(NULL), m_enginePowerProgress(NULL),
	m_tractionProgress(NULL), m_vehiclePreview(NULL), m_currentVehicleIndex(0),
	m_bgMusic(NULL), m_clickSE(NULL), m_hoverSE(NULL), m_vehicleNameLabel(NULL), m_infoLabel(NULL)
{
	m_uiFileName = "vehicle_select.layout";
}

VehicleSelectScreen::~VehicleSelectScreen()
{
}

void VehicleSelectScreen::enter()
{
	m_ogrePlatform->getRenderManagerPtr()->setSceneManager(m_sceneMgr);

	static const String resourceName = "Essential";
	if (!ResourceGroupManager::getSingleton().isResourceGroupInitialised(resourceName))
		ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceName);
	if (!ResourceGroupManager::getSingleton().isResourceGroupLoaded(resourceName))
		ResourceGroupManager::getSingleton().loadResourceGroup(resourceName);

	const String& vehicleNames = GODLoader::getSingleton().getGameObjectDefinitionByTag("vehicle_list")->getParameters()["vehicles"];
	m_vehicleList = StringUtil::split(vehicleNames, " ");

	m_clickSE = AudioManager::getSingleton().createSoundEffect("click.ogg");
	m_hoverSE = AudioManager::getSingleton().createSoundEffect("clickalt.ogg");

	buildUI();
}

void VehicleSelectScreen::buildUI()
{
	m_logger->logMessage("VehicleSelectScreen::buildUI()");

	m_guiLayout = LayoutManager::getInstance().loadLayout(m_uiFileName);

	ImageBox* parentBG = Gui::getInstance().findWidget<ImageBox>("backImg");
	parentBG->setSize(RenderManager::getInstance().getViewSize());
	parentBG->setImageTexture("blue_back.jpg");

	m_vehiclePreview = Gui::getInstance().findWidget<ImageBox>("vehicleModel");

	m_enginePowerProgress = Gui::getInstance().findWidget<ProgressBar>("enginePowerProgress");
	m_maxSpeedProgress = Gui::getInstance().findWidget<ProgressBar>("maxSpeedProgress");
	m_tractionProgress = Gui::getInstance().findWidget<ProgressBar>("tractionProgress");
	m_accelProgress = Gui::getInstance().findWidget<ProgressBar>("accelProgress");

	m_enginePowerProgress->setProgressRange(100);
	m_maxSpeedProgress->setProgressRange(100);
	m_tractionProgress->setProgressRange(100);
	m_accelProgress->setProgressRange(100);

	m_vehicleNameLabel = Gui::getInstance().findWidget<TextBox>("vehicleNameLabel");
	m_infoLabel = Gui::getInstance().findWidget<TextBox>("infoLabel");

	m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg", AudioType::SOUND_2D_LOOPING);
	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);

	m_backBtn = Gui::getInstance().findWidget<Button>("backBtn");
	m_selectBtn = Gui::getInstance().findWidget<Button>("selectBtn");
	m_prevBtn = Gui::getInstance().findWidget<ImageBox>("prevBtn");
	m_nextBtn = Gui::getInstance().findWidget<ImageBox>("nextBtn");
	
	m_selectBtn->eventMouseButtonClick += newDelegate(this, &VehicleSelectScreen::buttonClicked);
	m_prevBtn->eventMouseButtonClick += newDelegate(this, &VehicleSelectScreen::buttonClicked);
	m_backBtn->eventMouseButtonClick += newDelegate(this, &VehicleSelectScreen::buttonClicked);
	m_nextBtn->eventMouseButtonClick += newDelegate(this, &VehicleSelectScreen::buttonClicked);

	m_selectBtn->eventMouseSetFocus += newDelegate(this, &VehicleSelectScreen::buttonHovered);
	m_prevBtn->eventMouseSetFocus += newDelegate(this, &VehicleSelectScreen::buttonHovered);
	m_backBtn->eventMouseSetFocus += newDelegate(this, &VehicleSelectScreen::buttonHovered);
	m_nextBtn->eventMouseSetFocus += newDelegate(this, &VehicleSelectScreen::buttonHovered);

	inflateVehicleInfo();
}

void VehicleSelectScreen::buttonHovered(WidgetPtr sender, WidgetPtr old)
{
	m_hoverSE->play();
}

void VehicleSelectScreen::update(Ogre::Real delta)
{
}

void VehicleSelectScreen::buttonClicked(WidgetPtr sender)
{
	if (sender == m_backBtn)
		backButtonClicked();
	else if (sender == m_selectBtn)
		selectButtonClicked();
	else if (sender == m_nextBtn)
		nextButtonClicked();
	else if (sender == m_prevBtn)
		prevButtonClicked();
}

void VehicleSelectScreen::inflateVehicleInfo()
{
	GOD* god = GODLoader::getSingleton().getGameObjectDefinitionByTag(m_vehicleList[m_currentVehicleIndex]);
	auto& defs = god->getParameters();
	const String& vehicleMesh = defs["meshFile"];
	const String& niceName = defs["nice_name"];
	const String& vehicleModelImage = defs["vehicle_preview"];

	m_vehiclePreview->setImageTexture(vehicleModelImage);
	m_vehicleNameLabel->setCaption(niceName);

	// Hand-coded vehicle parameters
	const int enginePower = StringConverter::parseInt(defs["engineProgressValue"]);
	const int speedVal = StringConverter::parseInt(defs["maxSpeedProgressValue"]);;
	const int accelVal = StringConverter::parseInt(defs["accelProgressValue"]);;
	const int tractionVal = StringConverter::parseInt(defs["tractionProgressValue"]);;

	m_enginePowerProgress->setProgressPosition(enginePower);
	m_maxSpeedProgress->setProgressPosition(speedVal);
	m_accelProgress->setProgressPosition(accelVal);
	m_tractionProgress->setProgressPosition(tractionVal);
}

void VehicleSelectScreen::pause()
{
	Screen::pause();

	if (m_guiLayout.size() > 0)
	{
		LayoutManager::getInstance().unloadLayout(m_guiLayout);
		m_guiLayout.clear();
	}

	m_bgMusic->stop();
}

void VehicleSelectScreen::resume()
{
	Screen::resume();

	if (!m_bgMusic)
	{
		m_bgMusic = AudioManager::getSingleton().createMusic("menu.ogg");
	}

	m_bgMusic->play();
	m_bgMusic->setVolume(0.5);
}

void VehicleSelectScreen::leave()
{
	m_logger->logMessage("VehicleSelectScreen::leave()");

	if (m_guiLayout.size() > 0)
	{
		LayoutManager::getInstance().unloadLayout(m_guiLayout);
		m_guiLayout.clear();
	}

	m_vehicleList.clear();

	m_bgMusic->stop();
	//AudioManager::getSingleton().release(m_bgMusic);
}

bool VehicleSelectScreen::keyPressed(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyPress(KeyCode::Enum(e.key), e.text);

	return true;
}

bool VehicleSelectScreen::keyReleased(const OIS::KeyEvent& e)
{
	InputManager::getInstance().injectKeyRelease(KeyCode::Enum(e.key));

	switch (e.key)
	{
	case OIS::KC_RETURN:
		selectButtonClicked();
		break;
	case OIS::KC_LEFT:
		prevButtonClicked();
		break;
	case OIS::KC_RIGHT:
		nextButtonClicked();
		break;
	default:
		break;
	}

	return true;
}

bool VehicleSelectScreen::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMousePress(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}

bool VehicleSelectScreen::mouseMoved(const OIS::MouseEvent& e)
{
	InputManager::getInstance().injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);

	return true;
}

bool VehicleSelectScreen::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	InputManager::getInstance().injectMouseRelease(e.state.X.abs, e.state.Y.abs, MouseButton::Enum(id));

	return true;
}

void VehicleSelectScreen::backButtonClicked()
{
	m_logger->logMessage("VehicleSelectScreen::backButtonClicked()");

	m_screenManager->changeScreen(ScreenTag::TRACK_SELECT, true);
}

void VehicleSelectScreen::selectButtonClicked()
{
	m_logger->logMessage("VehicleSelectScreen::selectButtonClicked()");

	m_infoLabel->setCaption("Please wait while game data is loaded...");

	Player* player = PlayerManager::getSingleton().getLocalPlayer();
	assert(player != NULL && "PlayerManager::getLocalPlayer() fails!");
	player->setSelectedVehicleName(m_vehicleList[m_currentVehicleIndex]);

	m_bgMusic->stop();

	if (Game::getGameMode() == GameMode::PRACTICE)
	{
		m_screenManager->changeScreen(ScreenTag::GAME_PLAY, false);
	}
	else // GameMode == GameMode::MULTI_PLAYER)
	{
		m_screenManager->changeScreen(ScreenTag::GAME_LOBBY, true);
	}
}

void VehicleSelectScreen::nextButtonClicked()
{
	m_logger->logMessage("VehicleSelectScreen::nextButtonClicked()");

	static const size_t length = m_vehicleList.size();
	if (m_currentVehicleIndex + 1 < length)
		m_currentVehicleIndex++;
	else
		m_currentVehicleIndex = 0;

	inflateVehicleInfo();
}

void VehicleSelectScreen::prevButtonClicked()
{
	m_logger->logMessage("VehicleSelectScreen::prevButtonClicked()");

	if (m_currentVehicleIndex - 1 >= 0)
		m_currentVehicleIndex--;
	else
	{
		static const size_t length = m_vehicleList.size();
		m_currentVehicleIndex = length - 1;
	}

	inflateVehicleInfo();
}