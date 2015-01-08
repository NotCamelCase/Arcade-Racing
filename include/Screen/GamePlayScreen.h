#pragma once

#include "Screen.h"

#include <vector>

#include "Event\IEventListener.h"

class Music;
class Player;
struct PlayerStats;
class SoundEffect;
class WorldManager;
class ScreenManager;
class PlayerManager;
class GameObjectManager;

namespace Ogre { class SceneNode; class ManualObject; class Timer; }
namespace MyGUI { class TextBox; }

enum GameState
{
	INITIALIZATION,
	ON_COUNTDOWN,
	RACE_START,
	IN_GAME,
	END_GAME
};

class GamePlayScreen : public Screen, public IEventListener
{
public:
	GamePlayScreen(ScreenManager* sm, ScreenTag tag);
	virtual ~GamePlayScreen();

	virtual void enter();
	virtual void buildUI();
	virtual void update(Ogre::Real delta);
	virtual void pause();
	virtual void resume();
	virtual void leave();

	virtual bool keyPressed(const OIS::KeyEvent& e);
	virtual bool keyReleased(const OIS::KeyEvent& e);

	virtual bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
	virtual bool mouseMoved(const OIS::MouseEvent& e);
	virtual bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);

	virtual void onEvent(int type, void* data = 0);

private:
	GameObjectManager* m_gameObjManager;
	WorldManager* m_worldManager;
	PlayerManager* m_playerManager;
	PlayerStats* m_hudStats;
	Player* m_localPlayer;

	Ogre::Timer* m_timer;

	Ogre::SceneNode* m_gaugeNode, *m_needleNode;
	Ogre::SceneNode* m_camNode;
	Ogre::ManualObject* m_gaugeMO, *m_needleMO;

	int m_trackMaxLap;

	bool m_endUIShown;

	Music* m_ambientMusic;
	SoundEffect* m_countdownSE;
	SoundEffect* m_buttonClickSE;
	SoundEffect* m_buttonHoverSE;
	SoundEffect* m_lapCompleteSE;

	// HUD elements
	MyGUI::TextBox* m_lapLabel;
	MyGUI::TextBox* m_timeDiffLabel;
	MyGUI::TextBox* m_posLabel;
	MyGUI::TextBox* m_raceTimeLabel;
	MyGUI::TextBox* m_playerNameLabel;

	void buttonHovered(MyGUI::WidgetPtr sender, MyGUI::WidgetPtr old);
	void buttonClicked(MyGUI::WidgetPtr sender);
	void backToMainMenu();
	void raceAgain();

	void handleGameInit();
	void handleCountDown(Ogre::Real delta);
	void handleRacing(Ogre::Real delta);
	void handleEndGame();

	void updateHUD(Ogre::Real delta);

	void onPlayerLapComplete(Player* player);
	void onPlayerTrackComplete(Player* player);

	void switchToGamePlayScreen();

	GameState m_currentState;
};