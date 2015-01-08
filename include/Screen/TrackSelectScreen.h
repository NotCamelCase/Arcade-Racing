#pragma once

#include "Screen.h"

#include <OgreStringVector.h>

class Music;
class ScreenManager;
class PlayerManager;
class SoundEffect;

class TrackSelectScreen : public Screen
{
public:
	TrackSelectScreen(ScreenManager* sm, ScreenTag tag);
	~TrackSelectScreen();

	virtual void enter();
	virtual void buildUI();
	virtual void update(Ogre::Real delta);
	virtual void leave();
	virtual void resume();
	virtual void pause();

	virtual bool keyPressed(const OIS::KeyEvent& e);
	virtual bool keyReleased(const OIS::KeyEvent& e);

	virtual bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
	virtual bool mouseMoved(const OIS::MouseEvent& e);
	virtual bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);

private:
	MyGUI::Button* m_backButton;
	MyGUI::Button* m_selectButton;

	MyGUI::ImageBox* m_prevButton;
	MyGUI::ImageBox* m_nextButton;

	MyGUI::TextBox* m_trackNiceName;
	MyGUI::TextBox* m_trackLaps;

	MyGUI::ImageBox* m_trackPreview;

	Ogre::StringVector m_trackList;

	PlayerManager* m_playerManager;

	SoundEffect* m_clickSE;
	SoundEffect* m_hoverSE;
	Music* m_bgMusic;

	int m_currentTrackIndex;
	bool inflateTrackInfo();

	void buttonHovered(MyGUI::WidgetPtr sender, MyGUI::WidgetPtr old);
	void buttonClicked(MyGUI::WidgetPtr sender);
	void nextTrackClicked();
	void prevTrackClicked();
	void backToMenuClicked();
	void selectTrackClicked();
};