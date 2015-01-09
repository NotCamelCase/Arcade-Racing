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