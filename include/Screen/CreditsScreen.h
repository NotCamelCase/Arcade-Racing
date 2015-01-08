#pragma once

#include "Screen.h"

class Music;

class CreditsScreen : public Screen
{
public:
	CreditsScreen(ScreenManager* sm, ScreenTag tag);
	~CreditsScreen();

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
	Music* m_bgMusic;

	MyGUI::ImageBox* m_bgImage;
};