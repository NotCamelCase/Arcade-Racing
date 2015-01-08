#pragma once

#include <OgreStringVector.h>

#include "Screen.h"

class Music;
class SoundEffect;
class ScreenManager;

class VehicleSelectScreen : public Screen
{
public:
	VehicleSelectScreen(ScreenManager* sm, ScreenTag tag);
	~VehicleSelectScreen();

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
	MyGUI::ImageBox* m_vehiclePreview;
	MyGUI::ImageBox* m_prevBtn;
	MyGUI::ImageBox* m_nextBtn;

	MyGUI::ProgressBar* m_enginePowerProgress;
	MyGUI::ProgressBar* m_maxSpeedProgress;
	MyGUI::ProgressBar* m_tractionProgress;
	MyGUI::ProgressBar* m_accelProgress;

	MyGUI::Button* m_backBtn;
	MyGUI::Button* m_selectBtn;
	
	MyGUI::TextBox* m_infoLabel;
	MyGUI::TextBox* m_vehicleNameLabel;

	Music* m_bgMusic;
	SoundEffect* m_clickSE;
	SoundEffect* m_hoverSE;

	void buttonHovered(MyGUI::WidgetPtr sender, MyGUI::WidgetPtr old);
	void buttonClicked(MyGUI::WidgetPtr sender);
	void backButtonClicked();
	void selectButtonClicked();
	void nextButtonClicked();
	void prevButtonClicked();

	void inflateVehicleInfo();

	int m_currentVehicleIndex;

	Ogre::StringVector m_vehicleList;
};