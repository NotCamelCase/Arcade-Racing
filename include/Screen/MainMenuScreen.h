#pragma once

#include "Screen.h"

class SoundEffect;
class Music;

class MainMenuScreen : public Screen
{
public:
	MainMenuScreen(ScreenManager* sm, ScreenTag tag);
	~MainMenuScreen();

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

private:
	MyGUI::Button* m_singlePlayerButton;
	MyGUI::Button* m_multiplayerButton;
	MyGUI::Button* m_creditsButton;
	MyGUI::Button* m_exitButton;
	MyGUI::ImageBox* m_bgImage;

	SoundEffect* m_clickSound;
	SoundEffect* m_hoverSound;
	Music* m_bgMusic;

	void buttonClicked(MyGUI::WidgetPtr sender);
	void buttonHovered(MyGUI::WidgetPtr sender, MyGUI::WidgetPtr old);

	void enterPracticeMode();
	void enterMultiplayer();
	void showCredits();
	void exitGame();
};