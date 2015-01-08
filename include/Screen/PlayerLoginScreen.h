#pragma once

#include "Screen.h"

class Music;

namespace MyGUI { class ImageBox; class Button; class TextBox; }

class PlayerLoginScreen : public Screen
{
public:
	PlayerLoginScreen(ScreenManager* sm, ScreenTag tag);
	~PlayerLoginScreen();

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
	MyGUI::ImageBox* m_bgImage;

	MyGUI::Button* m_loginButton;
	MyGUI::TextBox* m_loginInfoLabel;
	MyGUI::EditBox* m_usernameBox;
	MyGUI::EditBox* m_passwordBox;

	void buttonClicked(MyGUI::WidgetPtr sender);

	Music* m_bgMusic;

	void loginGame();
};