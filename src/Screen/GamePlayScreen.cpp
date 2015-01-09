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

#include "Screen\GamePlayScreen.h"

#include <Ogre.h>

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#include "Common-cpp\inc\Common.h"

#include "Audio\Music.h"
#include "World\World.h"
#include "Audio\SoundEffect.h"
#include "World\WorldManager.h"
#include "Network\GameClient.h"
#include "Audio\AudioManager.h"
#include "Screen\ScreenManager.h"
#include "Network\NetworkManager.h"
#include "Physics\PhysicsManager.h"
#include "World\GameObjectManager.h"
#include "Network\NetworkTypenames.h"

#include "Player\Player.h"
#include "Physics\Vehicle.h"
#include "Player\RaceManager.h"
#include "Event\EventManager.h"
#include "Player\PlayerManager.h"

#include "Game.h"
#include "Utils.h"
#include "InputMap.h"
#include "GODLoader.h"

using namespace Ogre;

#define DYNAMIC_SHADOWMAP_TECHNIQUE_SUPPORT 0

#define DEBUG_UI
#undef DEBUG_UI
#define LANDSCAPE
#undef LANDSCAPE

#define CAM_ROT_SPEED 5.25
#define CAM_MOVE_SPEED 150

#define COUNTDOWN_DURATION 3000

#define CHASE_CAM_AUTO 1
#define CHASE_CAM_LIMITED 0

using namespace ExitGames;

GamePlayScreen::GamePlayScreen(ScreenManager *sm, ScreenTag tag)
	: Screen(sm, tag), m_worldManager(NULL), m_gameObjManager(NULL),
	m_ambientMusic(NULL), m_playerManager(NULL), m_localPlayer(NULL),
	m_timer(NULL), m_countdownSE(NULL), m_lapCompleteSE(NULL), m_buttonClickSE(NULL), m_buttonHoverSE(NULL),
	m_lapLabel(NULL), m_posLabel(NULL), m_playerNameLabel(NULL), m_raceTimeLabel(NULL),
	m_hudStats(NULL), m_trackMaxLap(0), m_gaugeMO(NULL), m_camNode(NULL), m_gaugeNode(NULL), m_needleMO(NULL), m_needleNode(NULL), m_timeDiffLabel(NULL),
	m_endUIShown(false), m_currentState(GameState::INITIALIZATION)
{
	m_gameObjManager = new GameObjectManager(m_sceneMgr);
	m_playerManager = &PlayerManager::getSingleton();

	// Loading UI
	m_uiFileName = "loading_screen.layout";

	// Manually update render window as resources/processes are done
	Game::getSingleton().getWindow()->setAutoUpdated(false);
	Screen::readjustMouseVisibility(false);
}

GamePlayScreen::~GamePlayScreen()
{
}

void GamePlayScreen::enter()
{
	m_logger->logMessage("GamePlayScreen::enter()");

	m_ogrePlatform->getRenderManagerPtr()->setSceneManager(m_sceneMgr);

	// Load preloader UI
	m_guiLayout = MyGUI::LayoutManager::getInstance().loadLayout(m_uiFileName);
	MyGUI::ImageBox* back = m_gui->findWidget<MyGUI::ImageBox>("background");
	if (back)
	{
		back->setSize(MyGUI::RenderManager::getInstance().getViewSize());
	}
	Game::getSingleton().getWindow()->update();

	MyGUI::ProgressBar* loadingBar = m_gui->findWidget<MyGUI::ProgressBar>("loadingBar");
	MyGUI::TextBox* descLabel = m_gui->findWidget<MyGUI::TextBox>("resLabel");

	loadingBar->setProgressRange(100);
	loadingBar->setProgressPosition(15);
	descLabel->setCaption("Fetching game resources");
	Game::getSingleton().getWindow()->update();

	const String trackFileName = GODLoader::getSingleton().getGameObjectDefinitionValue(RaceManager::getSingleton().getTrackData()->objName, "file_name");
	const String trackFileLocation = StringUtil::split(trackFileName, ".")[0]; // (trackX) - (scene)

	static const String resName = "InGame";
	ResourceGroupManager::getSingleton().addResourceLocation("media/mesh/" + trackFileLocation, "FileSystem", resName);

	loadingBar->setProgressPosition(22);
	descLabel->setCaption("Loading world data");
	Game::getSingleton().getWindow()->update();

	if (!ResourceGroupManager::getSingleton().isResourceGroupInitialised(resName))
	{
		ResourceGroupManager::getSingleton().initialiseResourceGroup(resName);
	}

	if (!ResourceGroupManager::getSingleton().isResourceGroupLoaded(resName))
	{
		ResourceGroupManager::getSingleton().loadResourceGroup(resName);
	}

	loadingBar->setProgressPosition(41);
	descLabel->setCaption("Setting up scene hierarchy");
	Game::getSingleton().getWindow()->update();

	m_camera->setNearClipDistance(5.f);
	m_camera->setFarClipDistance(10000.f);
	m_camera = m_sceneMgr->getCamera("Camera3");
	m_camera->setAutoAspectRatio(true);

	m_localPlayer = m_playerManager->getLocalPlayer();
	assert(m_localPlayer != NULL && "Local player couldn't be created!");;
	m_hudStats = m_localPlayer->getPlayerStats();

	m_trackMaxLap = RaceManager::getSingleton().getTrackData()->lapCount;

	const String& pn = m_localPlayer->getName();
	const String& vn = m_localPlayer->getSelectedVehicleName();
	const String& tn = RaceManager::getSingleton().getTrackData()->objName;
	const String& skyBoxName = GODLoader::getSingleton().getGameObjectDefinitionValue(tn, "sky_box");
	const String& skyDomeName = GODLoader::getSingleton().getGameObjectDefinitionValue(tn, "sky_dome");
	const bool inNeedOfLight = StringConverter::parseBool(GODLoader::getSingleton().getGameObjectDefinitionValue(tn, "needs_additional_light"));
	if (inNeedOfLight)
	{
		// Create additional sun light
		Light* sun2 = m_sceneMgr->createLight("Sun2");
		sun2->setType(Light::LightTypes::LT_DIRECTIONAL);
		sun2->setPosition(-359, 57, 0);
		sun2->setDirection(0, -1, -1);
		sun2->setPowerScale(1.f);
		sun2->setDiffuseColour(1, 1, 1);
		sun2->setAttenuation(10000, 1, 0, 0);
		sun2->setCastShadows(false);
	}

	if (DYNAMIC_SHADOWMAP_TECHNIQUE_SUPPORT)
	{
		m_sceneMgr->setShadowTextureSelfShadow(true);
		m_sceneMgr->setShadowTextureCasterMaterial("Ogre/DepthShadowmap/Caster/Float");
		m_sceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_R);
		m_sceneMgr->setShadowCasterRenderBackFaces(false);
		m_sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		m_sceneMgr->setSkyBox(true, skyBoxName);
	}
	else {
		m_sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
		m_sceneMgr->setShadowTextureSize(4096);
		m_sceneMgr->setShadowFarDistance(500);
		Ogre::ShadowCameraSetupPtr shadowTech = ShadowCameraSetupPtr(OGRE_NEW Ogre::LiSPSMShadowCameraSetup);
		m_sceneMgr->setShadowCameraSetup(shadowTech);
		if (!skyBoxName.empty())
			m_sceneMgr->setSkyBox(true, skyBoxName, 2500.f);
		else if (!skyDomeName.empty())
			m_sceneMgr->setSkyDome(true, skyDomeName);
	}

	const Ogre::ColourValue trackAmbient = StringConverter::parseColourValue(GODLoader::getSingleton().getGameObjectDefinitionValue(tn, "scene_ambient"));
	m_sceneMgr->setAmbientLight(trackAmbient);

	// Default player controls
	InputMap::mapInput(InputType::ACCELERATE, OIS::KC_UP);
	InputMap::mapInput(InputType::BRAKE, OIS::KC_DOWN);
	InputMap::mapInput(InputType::STEER_LEFT, OIS::KC_LEFT);
	InputMap::mapInput(InputType::STEER_RIGHT, OIS::KC_RIGHT);

	loadingBar->setProgressPosition(59);
	descLabel->setCaption("Assembling game objects");
	Game::getSingleton().getWindow()->update();

	m_worldManager = new WorldManager(m_sceneMgr);
	World* currentWorld = m_worldManager->loadWorld(trackFileName);

	loadingBar->setProgressPosition(75);
	descLabel->setCaption("Building track structure");
	Game::getSingleton().getWindow()->update();

	m_gameObjManager->buildGameWorld(currentWorld->getWorldObjects());
	currentWorld->setValidated(true);

	loadingBar->setProgressPosition(85);
	descLabel->setCaption("Creating vehicle(s)");
	Game::getSingleton().getWindow()->update();

	EventManager::getSingleton().registerListener(EventType::TRACK_COMPLETE, this);
	EventManager::getSingleton().registerListener(EventType::PLAYER_VEHICLE_READY, this);
	EventManager::getSingleton().registerListener(EventType::LAP_COMPLETE, this);

	for (const auto& idPlayerPair : PlayerManager::getSingleton().getGamePlayers())
	{
		m_gameObjManager->createVehicle(idPlayerPair.second->getSelectedVehicleName(), idPlayerPair.first);
	}

	const int isPractice = Game::getGameMode() == GameMode::PRACTICE;
	if (!isPractice)
	{
		// TODO: Trigger an event that my GameClient finished loading and is ready to enter the state ON_COUNTDOWN
		GameClient* myClient = NetworkManager::getSingleton().getGameClientByPlayer(m_localPlayer);
		myClient->setClientState(ClientState::READY_FOR_COUNTDOWN);
		NetworkManager::getSingleton().broadcastGameEvent(true, NetworkTypenames::EV_GAME_CLIENT_POST_LOADING);

		loadingBar->setProgressPosition(92);
		descLabel->setCaption("Synchronizing game clients");
		Game::getSingleton().getWindow()->update();
	}

	m_logger->logMessage("GamePlayScreen initialized");
}

void GamePlayScreen::switchToGamePlayScreen()
{
#if CHASE_CAM_AUTO
	SceneNode* localVehicleNode = m_gameObjManager->getVehicleByID(m_localPlayer->getID())->getNode();
	m_camNode = localVehicleNode->createChildSceneNode("VehicleTrackNode");
	m_camNode->attachObject(m_camera);
	m_camNode->translate(0, 10, -20, Node::TransformSpace::TS_PARENT);
	m_camera->setAutoTracking(true, localVehicleNode);
#endif

	MyGUI::LayoutManager::getInstance().unloadLayout(m_guiLayout);
	m_guiLayout.clear();
	//Game::getSingleton().getWindow()->update();

	m_uiFileName = "player_hud.layout";
	buildUI();

	m_ambientMusic = AudioManager::getSingleton().createMusic("SAA Ambience.ogg", AudioType::SOUND_2D_LOOPING);
	m_ambientMusic->play();
	m_ambientMusic->setVolume(0.80);

	m_screenManager->destroyDeactiveScreens();

	//m_sceneMgr->setFog(Ogre::FOG_LINEAR, ColourValue(0.90f, 0.90f, 0.90f), 0, 50, 100);

	Game::getSingleton().getWindow()->setAutoUpdated(true);
}

void GamePlayScreen::onEvent(int type, void* data)
{
	switch (type)
	{
	case EventType::LAP_COMPLETE:
		onPlayerLapComplete(static_cast<Player*> (data));
		break;
	case EventType::TRACK_COMPLETE:
		onPlayerTrackComplete(static_cast<Player*> (data));
		break;
	case EventType::PLAYER_VEHICLE_READY:
		Player* gp = PlayerManager::getSingleton().getPlayerByID((NBID*) data);
		gp->setPlayerState(PlayerState::READY_FOR_COUNTDOWN);
		break;
	}
}

void GamePlayScreen::onPlayerLapComplete(Player* p)
{
	m_logger->logMessage("Player " + p->getName() + " completed lap");

	// Prevent sound chaos when 64(!) networked players race in because it's such an awesome game! :D
	if (!p->isLocal()) return;

	if (m_lapCompleteSE == NULL)
	{
		m_lapCompleteSE = AudioManager::getSingleton().createSoundEffect("lap_complete.wav");
	}

	m_lapCompleteSE->play();
}

void GamePlayScreen::onPlayerTrackComplete(Player* p)
{
	m_logger->logMessage("Player " + p->getName() + " completed game");
	p->setPlayerState(PlayerState::FINISHED_TRACK);

	if (Game::getGameMode() == GameMode::PRACTICE && p == m_localPlayer)
	{
		m_currentState = GameState::END_GAME;
	}
	else // Multiplayer mode
	{
		if (p == m_localPlayer)
		{
			// Broadcast the event that I did complete the track!

			m_currentState = GameState::END_GAME;
		}
		else
		{
			// Broadcast the event that one of network players completed the track!
		}
	}
}

void GamePlayScreen::update(Real delta)
{
	switch (m_currentState)
	{
	case GameState::INITIALIZATION:
		handleGameInit();
		break;
	case GameState::ON_COUNTDOWN:
		handleCountDown(delta);
		break;
	case GameState::RACE_START:
		m_logger->logMessage("Race is starting...");
		EventManager::getSingleton().notify(EventType::START_OFF_FIRE);
		m_currentState = GameState::IN_GAME;
		break;
	case GameState::IN_GAME:
		handleRacing(delta);
		break;
	case GameState::END_GAME:
		handleEndGame();
		break;
	default:
		break;
	}

#if defined(DEBUG_UI)
	if (m_currentState == GameState::INITIALIZATION) return;

	const size_t pc = Game::getSingleton().getWindow()->getStatistics().triangleCount;
	MyGUI::TextBox* poly = m_gui->findWidget<MyGUI::TextBox>("polyLabel", false);
	if (poly)
		poly->setCaption("Poly count : " + StringConverter::toString(pc));

	const size_t bc = Game::getSingleton().getWindow()->getBatchCount();
	MyGUI::TextBox* batch = m_gui->findWidget<MyGUI::TextBox>("batchLabel", false);
	if (batch)
		batch->setCaption("Batch count : " + StringConverter::toString(bc));

	const float avrFPS = Game::getSingleton().getWindow()->getAverageFPS();
	MyGUI::TextBox* fps = m_gui->findWidget<MyGUI::TextBox>("FPS", false);
	if (fps)
		fps->setCaption("FPS : " + StringConverter::toString(avrFPS));

	PlayerStats* stats = m_localPlayer->getPlayerStats();
	const int speed = stats->vehicleSpeed;
	MyGUI::TextBox* speedLabel = m_gui->findWidget<MyGUI::TextBox>("speedLabel", false);
	if (speedLabel)
		speedLabel->setCaption("Speed : " + StringConverter::toString(speed) + " km/h");
#endif

	//m_logger->logMessage("Average FPS: " + Ogre::StringConverter::toString(Game::getSingleton().getWindow()->getAverageFPS()));

#if CHASE_CAM_LIMITED
	if (m_keyboard->isKeyDown(OIS::KC_W))
	{
		m_camera->moveRelative(Vector3(0, 0, -CAM_MOVE_SPEED * delta));
	}
	if (m_keyboard->isKeyDown(OIS::KC_S))
	{
		m_camera->moveRelative(Vector3(0, 0, CAM_MOVE_SPEED * delta));
	}
	if (m_keyboard->isKeyDown(OIS::KC_A))
	{
		m_camera->moveRelative(Vector3(-CAM_MOVE_SPEED * delta, 0, 0));
	}
	if (m_keyboard->isKeyDown(OIS::KC_D))
	{
		m_camera->moveRelative(Vector3(CAM_MOVE_SPEED * delta, 0, 0));
	}
	if (m_keyboard->isKeyDown(OIS::KC_Q))
	{
		m_camera->moveRelative(Vector3(0, -CAM_MOVE_SPEED * delta, 0));
	}
	if (m_keyboard->isKeyDown(OIS::KC_E))
	{
		m_camera->moveRelative(Vector3(0, CAM_MOVE_SPEED * delta, 0));
	}
	if (m_keyboard->isKeyDown(OIS::KC_Z))
	{
		m_camera->yaw(Radian(CAM_ROT_SPEED * delta));
	}
	if (m_keyboard->isKeyDown(OIS::KC_C))
	{
		m_camera->yaw(Radian(-CAM_ROT_SPEED * delta));
	}
	if (m_keyboard->isKeyDown(OIS::KC_R))
	{
		m_camera->pitch(Radian(-CAM_ROT_SPEED * delta));
	}
	if (m_keyboard->isKeyDown(OIS::KC_F))
	{
		m_camera->pitch(Radian(CAM_ROT_SPEED * delta));
	}
#endif
	}

void GamePlayScreen::pause()
{
	Screen::pause();

	EventManager::getSingleton().unregisterListener(EventType::TRACK_COMPLETE, this);
	EventManager::getSingleton().unregisterListener(EventType::PLAYER_VEHICLE_READY, this);
	EventManager::getSingleton().unregisterListener(EventType::LAP_COMPLETE, this);
}

void GamePlayScreen::resume()
{
	Screen::resume();

	EventManager::getSingleton().registerListener(EventType::TRACK_COMPLETE, this);
	EventManager::getSingleton().registerListener(EventType::PLAYER_VEHICLE_READY, this);
	EventManager::getSingleton().registerListener(EventType::LAP_COMPLETE, this);
}

void GamePlayScreen::leave()
{
	// FIXME : Debugging GUI
#if defined(DEBUG_UI)
	m_gui->destroyWidget(m_gui->findWidget<MyGUI::TextBox>("FPS", true));
	m_gui->destroyWidget(m_gui->findWidget<MyGUI::TextBox>("speedLabel", true));
	m_gui->destroyWidget(m_gui->findWidget<MyGUI::TextBox>("polyLabel", true));
	m_gui->destroyWidget(m_gui->findWidget<MyGUI::TextBox>("batchLabel", true));
#else
	// FIXME: Destroy Gauge & Needle
	//if (m_gaugeMO) m_sceneMgr->destroyRibbonTrail(m_gaugeMO);
	//if(m_needleMO) m_sceneMgr->destroyManualObject(m_needleMO);
#endif

	if (!m_guiLayout.empty())
	{
		MyGUI::LayoutManager::getInstance().unloadLayout(m_guiLayout);
		m_guiLayout.clear();
	}

	AudioManager::getSingleton().release(m_ambientMusic);

	EventManager::getSingleton().unregisterListener(EventType::TRACK_COMPLETE, this);
	EventManager::getSingleton().unregisterListener(EventType::PLAYER_VEHICLE_READY, this);
	EventManager::getSingleton().unregisterListener(EventType::LAP_COMPLETE, this);

	ResourceGroupManager::getSingleton().clearResourceGroup("InGame");

	Screen::readjustMouseVisibility(true);

	SAFE_DELETE(m_timer);
	SAFE_DELETE(m_gameObjManager);
	SAFE_DELETE(m_worldManager);
	m_logger->logMessage("Destroyed GamePlayScreen");
}

void GamePlayScreen::buildUI()
{
#if defined (DEBUG_UI)
	static const int w = Game::getSingleton().getWindow()->getWidth();
	static const int h = Game::getSingleton().getWindow()->getHeight();

	MyGUI::TextBox* fpsLogger = m_gui->createWidget<MyGUI::TextBox>("TextBox", 0, 0, 150, 50,
		MyGUI::Align::Right | MyGUI::Align::Bottom, "Main", "FPS");
	fpsLogger->setTextColour(MyGUI::Colour::White);
	fpsLogger->setFontHeight(25);

	MyGUI::TextBox* currentSpeed = m_gui->createWidget<MyGUI::TextBox>("TextBox", 0,
		Game::getSingleton().getWindow()->getHeight() - 25, 200, 25,
		MyGUI::Align::Right | MyGUI::Align::Bottom, "Main", "speedLabel");
	currentSpeed->setTextColour(MyGUI::Colour::White);
	currentSpeed->setFontHeight(25);

	MyGUI::TextBox* polyCount = m_gui->createWidget<MyGUI::TextBox>("TextBox", 0, 50, 350, 100,
		MyGUI::Align::Right | MyGUI::Align::Bottom, "Main", "polyLabel");
	polyCount->setTextColour(MyGUI::Colour::White);
	polyCount->setFontHeight(25);

	MyGUI::TextBox* batchCount = m_gui->createWidget<MyGUI::TextBox>("TextBox", 0, 25, 200, 100,
		MyGUI::Align::Right | MyGUI::Align::Bottom, "Main", "batchLabel");
	batchCount->setTextColour(MyGUI::Colour::White);
	batchCount->setFontHeight(25);
#elif defined (LANDSCAPE)
	m_logger->logMessage("Landscape MODE enabled");
#else
	m_guiLayout = MyGUI::LayoutManager::getInstance().loadLayout(m_uiFileName);
	MyGUI::ImageBox* back = m_gui->findWidget<MyGUI::ImageBox>("background");
	if (back)
	{
		back->setSize(MyGUI::RenderManager::getInstance().getViewSize());
		m_logger->logMessage(m_uiFileName + " resized");
	}

	m_posLabel = m_gui->findWidget<MyGUI::TextBox>("posLabel");
	m_lapLabel = m_gui->findWidget<MyGUI::TextBox>("currentLapLabel");
	m_playerNameLabel = m_gui->findWidget<MyGUI::TextBox>("playerNameLabel");
	m_raceTimeLabel = m_gui->findWidget<MyGUI::TextBox>("timeLabel");

	m_playerNameLabel->setCaption(m_localPlayer->getName());
	if (Game::getGameMode() == GameMode::PRACTICE)
	{
		m_posLabel->setCaption("1/1");
	}
	else
	{
		// Instead of local player's name,
		// Players with positions and timing will be listed above that label

		m_posLabel->setCaption("1/4"); // 4 is MAX_PLAYERS_COUNT
	}

	m_raceTimeLabel->setCaption("--:--.--");
	m_lapLabel->setCaption(StringConverter::toString(m_hudStats->currentLap) + "/" + "3");

	const String& gaugeMaterialName = GODLoader::getSingleton().
		getGameObjectDefinitionValue(m_localPlayer->getSelectedVehicleName(), "gauge_type");

	const Real sc = 0.5f;
	SceneNode* rootNode = m_sceneMgr->getRootSceneNode();

	m_gaugeMO = m_gameObjManager->create2DManualObject(gaugeMaterialName, m_sceneMgr, .42, true, false,
		sc, Vector2(0.f, 0.0f), Utils::RV_Hud, Utils::RQG_Hud1, true);
	m_gaugeNode = rootNode->createChildSceneNode();
	m_gaugeNode->attachObject(m_gaugeMO);
	m_gaugeNode->setVisible(true);

	const float w = 0.42;
	const float h = 0.42;
	m_gaugeNode->setPosition(1 - w, -h, 0);

	m_needleMO = m_gameObjManager->create2DManualObject(gaugeMaterialName, m_sceneMgr, .42, true, false,
		sc, Vector2(0.5f, 0.5f), Utils::RV_Hud, Utils::RQG_Hud3, true);
	m_needleNode = rootNode->createChildSceneNode();
	m_needleNode->attachObject(m_needleMO);
	m_needleNode->setVisible(true);

	// 7.33
	//   0: 135
	//  20: 113
	//  40: 89
	//  60: 65
	// 135 - (4 * 4)
	static const int initialNeedleAngle = 135;
	m_needleNode->setPosition(1 - w, -h, 0);

	Matrix3 rotMatrix;
	rotMatrix.FromAngleAxis(Vector3::UNIT_Z, Radian(Math::DegreesToRadians(135)));
	m_needleNode->setOrientation(rotMatrix);
#endif

	Screen::readjustMouseVisibility(false);

	m_logger->logMessage("GUI built up");
}

bool GamePlayScreen::keyPressed(const OIS::KeyEvent& e)
{
	MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(e.key), e.text);

	InputMap::reflectKeyPress(e);

	switch (e.key)
	{
#if _DEBUG
	case OIS::KC_F3:
		m_gameObjManager->getPhysicsManager()->setDebugging(true);
		break;
	case OIS::KC_F4:
		m_sceneMgr->showBoundingBoxes(true);
		break;
#endif
	default:
		break;
	}

	return true;
}

bool GamePlayScreen::keyReleased(const OIS::KeyEvent& e)
{
	MyGUI::InputManager::getInstance().injectKeyRelease(MyGUI::KeyCode::Enum(e.key));

	InputMap::reflectKeyRelease(e);

	switch (e.key)
	{
#if _DEBUG
	case OIS::KC_F3:
		m_gameObjManager->getPhysicsManager()->setDebugging(false);
		break;
	case OIS::KC_F4:
		m_sceneMgr->showBoundingBoxes(false);
		break;
	case OIS::KC_F6:
		m_currentState = GameState::RACE_START;
		break;
#endif
	case OIS::KC_F5:
		Game::getSingleton().getWindow()->writeContentsToTimestampedFile("ss", ".png");
		break;
	case OIS::KC_F10:
		m_screenManager->changeScreen(ScreenTag::MAIN_MENU, false);
		break;
	case OIS::KC_LSHIFT:
		if (m_currentState == GameState::IN_GAME)
			EventManager::getSingleton().notify(EventType::VEHICLE_UPSIDE_DOWN);
		break;
	default:
		break;
	}

	return true;
}

bool GamePlayScreen::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	MyGUI::InputManager::getInstance().injectMousePress(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));

	return true;
}

bool GamePlayScreen::mouseMoved(const OIS::MouseEvent& e)
{
	MyGUI::InputManager::getInstance().injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);

	return true;
}

bool GamePlayScreen::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	MyGUI::InputManager::getInstance().injectMouseRelease(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));

	return true;
}

void GamePlayScreen::handleGameInit()
{
	m_logger->logMessage("GamePlay::handleGameInit()");

	const bool vehiclesReady = PlayerManager::getSingleton().isReadyForCountDown();
	const bool clientsReady = NetworkManager::getSingleton().isReadyForCountDown();
	if (Game::getGameMode() == GameMode::PRACTICE)
	{
		m_currentState = vehiclesReady ? GameState::ON_COUNTDOWN : m_currentState;
	}
	else
	{
		m_currentState = (vehiclesReady && clientsReady) ? GameState::ON_COUNTDOWN : m_currentState;
	}
}

void GamePlayScreen::handleCountDown(Ogre::Real delta)
{
	if (m_timer == NULL) // Just completed initialization
	{
		switchToGamePlayScreen();

		m_countdownSE = AudioManager::getSingleton().createSoundEffect("countdown.mp3", AudioType::SOUND_2D);
		m_countdownSE->play();

		m_timer = new Ogre::Timer();
		m_logger->logMessage("CountDown started: " + StringConverter::toString(m_timer->getMilliseconds()));
	}

	const unsigned long timePassed = m_timer->getMilliseconds();
	if (timePassed >= COUNTDOWN_DURATION)
	{
		m_logger->logMessage("CountDown ended: " + StringConverter::toString(m_timer->getMilliseconds()));

		SAFE_DELETE(m_timer);

		m_countdownSE->stop();
		AudioManager::getSingleton().release(m_countdownSE);

		m_currentState = GameState::RACE_START;
	}
	else // Vehicle is IDLE
	{
		if (delta < 1)
		{
			m_gameObjManager->update(delta);
		}
	}
}

void GamePlayScreen::handleRacing(Real delta)
{
	m_gameObjManager->update(delta);
#if !defined (DEBUG_UI) || !defined(LANDSCAPE)
	updateHUD(delta);
#endif
}

void GamePlayScreen::handleEndGame()
{
	if (!m_endUIShown)
	{
		Screen::readjustMouseVisibility(true);

		if (m_gaugeMO != NULL) m_sceneMgr->destroyManualObject(m_gaugeMO);
		if (m_needleMO != NULL) m_sceneMgr->destroyManualObject(m_needleMO);
		m_logger->logMessage("Gauge and needle destroyed");

		if (!m_guiLayout.empty())
		{
			MyGUI::LayoutManager::getInstance().unloadLayout(m_guiLayout); // end_race.layout
			m_guiLayout.clear();
		}

		if (Game::getGameMode() == GameMode::PRACTICE) // Present different UIs for practivce & multiplayer end race!
		{
			m_uiFileName = "end_race_pr.layout";
			m_guiLayout = MyGUI::LayoutManager::getInstance().loadLayout(m_uiFileName);

			MyGUI::ImageBox* back = m_gui->findWidget<MyGUI::ImageBox>("background");
			if (back)
			{
				back->setSize(MyGUI::RenderManager::getInstance().getViewSize());
				m_logger->logMessage(m_uiFileName + " resized");
			}

			MyGUI::TextBox* raceTimeLabel = m_gui->findWidget<MyGUI::TextBox>("raceTimeLabel");
			raceTimeLabel->setCaption(Utils::stringifyStatsTiming(m_hudStats));

			// Back to Menu button
			MyGUI::ButtonPtr prButton = m_gui->findWidget<MyGUI::Button>("backToMenuButton");
			prButton->eventMouseButtonClick += MyGUI::newDelegate(this, &GamePlayScreen::buttonClicked);
			prButton->eventMouseSetFocus += MyGUI::newDelegate(this, &GamePlayScreen::buttonHovered);

			// Button for saving stats locally to xml
			prButton = m_gui->findWidget<MyGUI::Button>("gotoTracksButton");
			prButton->eventMouseButtonClick += MyGUI::newDelegate(this, &GamePlayScreen::buttonClicked);
			prButton->eventMouseSetFocus += MyGUI::newDelegate(this, &GamePlayScreen::buttonHovered);

			prButton = NULL;
		}
		else
		{
			m_uiFileName = "end_race_mp.layout";
			m_guiLayout = MyGUI::LayoutManager::getInstance().loadLayout(m_uiFileName);

			MyGUI::ImageBox* back = m_gui->findWidget<MyGUI::ImageBox>("background");
			if (back)
			{
				back->setSize(MyGUI::RenderManager::getInstance().getViewSize());
				m_logger->logMessage(m_uiFileName + " resized");
			}

			// TODO: Fill in m_multiEndUI with widgets to be used again
		}

		m_buttonClickSE = AudioManager::getSingleton().createSoundEffect("click.ogg");
		m_buttonHoverSE = AudioManager::getSingleton().createSoundEffect("clickalt.ogg");

		m_endUIShown = true;
	}
}

void GamePlayScreen::updateHUD(Ogre::Real delta)
{
#if !defined (DEBUG_UI) && !defined (LANDSCAPE)
	// FIXME: Create RaceStats; use its fields!
	m_lapLabel->setCaption(StringConverter::toString(m_hudStats->currentLap) + "/" + StringConverter::toString(m_trackMaxLap));
	if (Game::getGameMode() == GameMode::MULTI_PLAYER)
	{
		m_logger->logMessage("FIX ME!"); // Adjust player pos accordingly!
	}

	m_raceTimeLabel->setCaption(Utils::stringifyStatsTiming(m_hudStats));

	static Matrix3 rotMatrix;
	const int speed = std::min(240, m_hudStats->vehicleSpeed); // Clamp to gauge limit
	if (speed > 0)
	{
		rotMatrix.FromAngleAxis(Vector3::UNIT_Z, Radian(Math::DegreesToRadians(135 - (1.1 * speed))));
		m_needleNode->setOrientation(rotMatrix);
	}
	else
	{
		rotMatrix.FromAngleAxis(Vector3::UNIT_Z, Radian(Math::DegreesToRadians(135)));
		m_needleNode->setOrientation(rotMatrix);
	}
#endif
}

void GamePlayScreen::buttonClicked(MyGUI::WidgetPtr sender)
{
	if (m_buttonClickSE)
		m_buttonClickSE->play();

	if (sender->getName() == "backToMenuButton")
	{
		if (!PlayerManager::getSingleton().savePlayerStatsLocally(m_localPlayer))
		{
			m_logger->logMessage("Local player stats failed to save!");
		}

		backToMainMenu();
	}
	else if (sender->getName() == "gotoTracksButton")
	{
		if (!PlayerManager::getSingleton().savePlayerStatsLocally(m_localPlayer))
		{
			m_logger->logMessage("Local player stats failed to save!");
		}

		raceAgain();
	}
}

void GamePlayScreen::buttonHovered(MyGUI::WidgetPtr sender, MyGUI::WidgetPtr old)
{
	if (m_buttonHoverSE)
		m_buttonHoverSE->play();
}

void GamePlayScreen::backToMainMenu()
{
	m_logger->logMessage("GamePlayScreen::backToMainMenu()");

	m_screenManager->changeScreen(ScreenTag::MAIN_MENU, false);
}

void GamePlayScreen::raceAgain()
{
	m_logger->logMessage("GamePlayScreen::raceAgain()");

	m_screenManager->changeScreen(ScreenTag::TRACK_SELECT, false);
}