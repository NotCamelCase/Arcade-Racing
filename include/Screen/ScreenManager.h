#pragma once

#include <OgreWindowEventUtilities.h>
#include <OgreFrameListener.h>
#include <OgreResourceGroupManager.h>
#include <OgreLog.h>

#include <vector>

#include "Screen.h"

namespace MyGUI { class Gui; class OgrePlatform; }
namespace Ogre { class RenderWindow; }
namespace OIS { class InputManager; class Keyboard; class Mouse; }

class Game;

/*
Class that holds pointers to Screen classes created, updates and renders
one active screen
*/
class ScreenManager : public Ogre::FrameListener, public Ogre::WindowEventListener
{
public:
	explicit ScreenManager(OIS::InputManager* im);
	~ScreenManager();

	friend class Game;

	/* change to Screen by tag. If pausePrevious is false => previous screen is destroyed */
	void changeScreen(ScreenTag tag, bool pausePrevious);

	/* destroy non-active screens paused */
	void destroyDeactiveScreens();

	/* add screen already created to list */
	bool addScreen(Screen* screen);

	bool frameStarted(const Ogre::FrameEvent& e);
	bool frameRenderingQueued(const Ogre::FrameEvent& e);
	bool frameEnded(const Ogre::FrameEvent& e);

	OIS::Keyboard* getKeyboard() const { return m_keyboard; }
	OIS::Mouse* getMouse() const { return m_mouse; }

	static MyGUI::Gui* getGUI() { return ScreenManager::m_gui; }
	static MyGUI::OgrePlatform* getOgrePlatform() { return ScreenManager::m_ogrePlatform; }

	void setShutDown(bool v) { m_shutDown = v; }

protected:
	bool windowClosing(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);
	void windowResized(Ogre::RenderWindow* rw);

private:
	Game* m_game;

	Ogre::Log* m_logger;

	OIS::InputManager* m_inputMgr;
	OIS::Keyboard* m_keyboard;
	OIS::Mouse* m_mouse;

	Screen* m_activeScreen;
	std::vector<Screen*> m_screens;

	static MyGUI::Gui* m_gui;
	static MyGUI::OgrePlatform* m_ogrePlatform;

	bool m_shutDown;

	Screen* getScreen(ScreenTag tag);
};