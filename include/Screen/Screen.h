#pragma once

#include <OgrePrerequisites.h>

#include <OISMouse.h>
#include <OISKeyboard.h>

#include <MyGUI_WidgetDefines.h>

namespace Ogre { class SceneManager; class Camera; class Log; }
namespace MyGUI { class Gui; class OgrePlatform; }

class ScreenManager;

enum ScreenTag
{
	MAIN_MENU,
	TRACK_SELECT,
	VEHICLE_SELECT,
	GAME_PLAY,
	CREDITS,
	USER_LOGIN,
	GAME_LOBBY
};

class Screen : public OIS::KeyListener, public OIS::MouseListener
{
public:
	Screen(ScreenManager* sm, ScreenTag tag);
	virtual ~Screen();
	
	/* Initialize common screen data */
	virtual void enter() = 0;

	/* To be called explicitly after completing @enter() */
	virtual void buildUI() = 0;

	/* Update screen elements */
	virtual void update(Ogre::Real delta) = 0;

	/* Pause rendering scene and get rid of UI layout */
	virtual void pause();

	/* Resume rendering scene and re-build UI layout */
	virtual void resume();

	/* Call when leaving Screen to dispose of things created early */
	virtual void leave() = 0;
	
	ScreenTag getScreenTag() const { return m_screenTag; }

	Ogre::SceneManager* getSceneManager() const { return m_sceneMgr; }
	Ogre::Log* getLog() const { return m_logger; }
	Ogre::Camera* getCamera() const { return m_camera; }

	MyGUI::OgrePlatform* getOgrePlatform() const { return m_ogrePlatform; }
	MyGUI::Gui* getGUI() const { return m_gui; }
	MyGUI::VectorWidgetPtr getGUILayout() const { return m_guiLayout; }

	Ogre::String getLayoutFileName() const { return m_uiFileName; }
	void setLayoutFileName(const Ogre::String& name) { m_uiFileName = name; }

	bool isPaused() const { return m_paused; }
	void setPaused(bool v) { m_paused = v; }

	/* If true, mouse cursor is shown otherwise hidden */
	static void readjustMouseVisibility(bool set);

protected:
	ScreenManager* m_screenManager;
	const ScreenTag m_screenTag;
	bool m_paused;

	Ogre::SceneManager* m_sceneMgr;
	Ogre::Camera* m_camera;
	Ogre::Log* m_logger;

	OIS::Keyboard* m_keyboard;
	OIS::Mouse* m_mouse;

	MyGUI::Gui* m_gui;
	MyGUI::OgrePlatform* m_ogrePlatform;
	MyGUI::VectorWidgetPtr m_guiLayout;
	Ogre::String m_uiFileName;
};