#pragma once

#include "Screen.h"
#include "Event\IEventListener.h"

class SoundEffect;

class OnlineLobbyScreen : public Screen, public IEventListener
{
public:
	OnlineLobbyScreen(ScreenManager* sm, ScreenTag tag);
	~OnlineLobbyScreen();

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

	virtual void onEvent(int eventType, void* data = 0);

private:
	MyGUI::TextBox* m_infoLabel;

	MyGUI::Button* m_createRoomButton;
	MyGUI::Button* m_joinRoomButton;
	MyGUI::Button* m_joinRandomButton;

	void buttonClicked(MyGUI::WidgetPtr sender);
	void buttonHovered(MyGUI::WidgetPtr sender, MyGUI::WidgetPtr old);

	SoundEffect* m_clickSound;
	SoundEffect* m_hoverSound;

	void createRoomForPlayer();
	void joinRandomRoom();
	void joinSelectedRoom(int index);

	void activateLobbyUI();
	void deactivateLobbyUI();
};